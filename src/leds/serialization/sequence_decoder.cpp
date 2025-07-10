// sequence_injector.cpp

#include "../sequence_scheduler.h"
#include "protocol.pb.h"
#include <pb_decode.h>
#include <vector>
#include "debug.h"
#include "layer_decoder.h"
#include "sequence_decoder.h"

/**
 * @brief Decodes an animation from the provided protobuf stream.
 *
 * This static method is responsible for decoding an animation from a protobuf stream
 * and populating the corresponding Animation object. It is used as a callback function
 * during the decoding process of a sequence.
 *
 * @param stream The input stream from which the animation data is read.
 * @param field The field iterator pointing to the current field being decoded.
 * @param arg A pointer to the argument passed to the callback, which is expected to be a Sequence object.
 * @return true if the animation is successfully decoded, false otherwise.
 */
bool SequenceDecoder::decode_animation(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
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
  animation->firstTick = incomingAnimation.first_tick;
  animation->brightness = incomingAnimation.brightness;

  return true;  // Return true if decoding is successful
}

/**
 * @brief Decodes a sequence from the provided protobuf stream.
 *
 * This static method is responsible for decoding a sequence from a protobuf stream
 * and populating the corresponding Sequence object. It sets up the necessary callback
 * for decoding animations within the sequence.
 *
 * @param stream The input stream from which the sequence data is read.
 * @param sequence A pointer to the Sequence object that will be populated with the decoded data.
 * @return true if the sequence is successfully decoded, false otherwise.
 */
bool SequenceDecoder::decode(pb_istream_t* stream, Sequence* sequence) {
  protocol_Sequence incomingSequence = protocol_Sequence_init_zero;

  incomingSequence.animations.funcs.decode = SequenceDecoder::decode_animation;
  incomingSequence.animations.arg = sequence;


  if (!pb_decode(stream, protocol_Sequence_fields, &incomingSequence)) {
    debug("\033[1;31mFailed to decode sequence\033[0m\n", 0);
    return false;  // Return empty sequence if decoding fails
  }

  return true;
}
