// sequence_injector.cpp

#include "sequence_scheduler.h"
#include "protocol.pb.h"
#include <pb_decode.h>
#include <vector>
#include "debug.h"
#include "layers/layer_decoder.cpp"

#define MAX_ANIMATIONS 8
#define MAX_LAYERS 3
#define MAX_EFFECTVALUES 50



class SequenceDecoder {
  private:
  ISequenceScheduler* scheduler;

  static bool decode_animation(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    _protocol_Animation incomingAnimation = protocol_Animation_init_zero;
    Animation* animation = new Animation();
    Sequence* sequence = static_cast<Sequence*>(*arg);
    sequence->animations.push_back(animation);

    incomingAnimation.layers.funcs.decode = decode_layer;
    incomingAnimation.layers.arg = animation;

    // Decode the incomingAnimation from the stream
    if (!pb_decode(stream, protocol_Animation_fields, &incomingAnimation)) {
      debug("\033[1;31mFailed to decode animation\033[0m\n", 0);
      return false;  // Return false if decoding fails
    }

    animation->direction = incomingAnimation.direction == protocol_Direction_FORWARD ? Direction::FORWARD : Direction::BACKWARD;
    animation->tickDuration = incomingAnimation.duration;

    return true;  // Return true if decoding is successful
  }

  static bool decode_layer(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    _protocol_Layer incomingLayer = protocol_Layer_init_zero;

    std::vector<u32_t>* effectSet = new std::vector<u32_t>();

    incomingLayer.effect_set.funcs.decode = decode_effect_set;
    incomingLayer.effect_set.arg = effectSet;

    // Decode the incomingLayer from the stream
    if (!pb_decode(stream, protocol_Layer_fields, &incomingLayer)) {
      debug("\033[1;31mFailed to decode effect_set\033[0m\n", 0);
      return false;
    }

    ILayer* layer = LayerDecoder::decode(incomingLayer, *effectSet);

    if (layer == nullptr) {
      debug("\033[1;31mFailed to decode layer\033[0m\n", 0);
      return false;
    }

    Animation* animation = static_cast<Animation*>(*arg);
    animation->layers.push_back(layer);

    return true;
  }

  static bool decode_effect_set(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    std::vector<u32_t>* effect_set = static_cast<std::vector<u32_t>*>(*arg);

    while (stream->bytes_left) {
      uint32_t value;
      if (!pb_decode_varint32(stream, &value)) {
        debug("\033[1;31mFailed to decode effect_set\033[0m\n", 0);
        return false;
      }
      effect_set->push_back(value);
    }
    return true;
  }

  public:
  SequenceDecoder(ISequenceScheduler* scheduler) : scheduler(scheduler) {}

  bool decode(pb_istream_t* stream, Sequence* sequence) {
    protocol_Sequence incomingSequence = protocol_Sequence_init_zero;

    incomingSequence.animations.funcs.decode = decode_animation;
    incomingSequence.animations.arg = sequence;


    if (!pb_decode(stream, protocol_Sequence_fields, &incomingSequence)) {
      debug("\033[1;31mFailed to decode sequence\033[0m\n", 0);
      return false;  // Return empty sequence if decoding fails
    }

    sequence->brightness = incomingSequence.brightness;
    return true;
  }
};