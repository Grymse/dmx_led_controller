// sequence_injector.cpp

#include "layer_scheduler.h"
#include "protocol.pb.h"
#include <pb_decode.h>

#define MAX_ANIMATIONS 8
#define MAX_LAYERS 3
#define MAX_EFFECTVALUES 50

class SequenceDecoder {
  private:
  ILayerScheduler* scheduler;

  static bool decode_animation(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    _protocol_Animation animation = {};
    animation.layers.funcs.decode = decode_layer;

    // Decode the animation from the stream
    if (!pb_decode(stream, protocol_Animation_fields, &animation)) {
      printf("\033[1;31mFailed to decode animation\033[0m\n");
      return false;  // Return false if decoding fails
    }

    return true;  // Return true if decoding is successful
  }

  static bool decode_layer(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    _protocol_Layer layer = {};
    layer.effect_set.funcs.decode = decode_effect_set;

    // Decode the layer from the stream
    if (!pb_decode(stream, protocol_Layer_fields, &layer)) {
      printf("\033[1;31mFailed to decode layer\033[0m\n");
      return false;  // Return false if decoding fails
    }

    return true;  // Return true if decoding is successful
  }

  static bool decode_effect_set(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    _protocol_Layer layer = {};

    while (stream->bytes_left) {
      uint64_t value;
      if (!pb_decode_varint(stream, &value)) {
        printf("\033[1;31mFailed to decode effect_set\033[0m\n");
        return false;
      }
      printf("%lld\n", value);
    }
    return true;
  }

  public:
  SequenceDecoder(ILayerScheduler* scheduler) : scheduler(scheduler) {}

  void decode(pb_istream_t* stream) {
    protocol_Sequence sequence = {};
    sequence.animations.funcs.decode = decode_animation;

    if (!pb_decode(stream, protocol_Sequence_fields, &sequence)) {
      printf("\033[1;31mFailed to decode sequence\033[0m\n");
      return;
    }
  }
};