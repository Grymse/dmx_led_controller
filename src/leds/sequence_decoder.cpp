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

    std::vector<ILayer*>* layers = new std::vector<ILayer*>();
    incomingAnimation.layers.funcs.decode = LayerDecoder::decode_layer;
    incomingAnimation.layers.arg = &animation->layers;

    // Decode the incomingAnimation from the stream
    if (!pb_decode(stream, protocol_Animation_fields, &incomingAnimation)) {
      debug("\033[1;31mFailed to decode animation\033[0m\n", 0);
      return false;  // Return false if decoding fails
    }


    animation->direction = incomingAnimation.direction == protocol_Direction_FORWARD ? Direction::FORWARD : Direction::BACKWARD;
    animation->tickDuration = incomingAnimation.duration;

    return true;  // Return true if decoding is successful
  }

  public:
  SequenceDecoder(ISequenceScheduler* scheduler) : scheduler(scheduler) {}

  bool decode(pb_istream_t* stream, Sequence* sequence) {
    protocol_Sequence incomingSequence = protocol_Sequence_init_zero;

    incomingSequence.animations.funcs.decode = SequenceDecoder::decode_animation;
    incomingSequence.animations.arg = sequence;


    if (!pb_decode(stream, protocol_Sequence_fields, &incomingSequence)) {
      debug("\033[1;31mFailed to decode sequence\033[0m\n", 0);
      return false;  // Return empty sequence if decoding fails
    }


    sequence->brightness = incomingSequence.brightness;
    return true;
  }
};