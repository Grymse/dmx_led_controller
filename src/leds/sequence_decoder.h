#pragma once

#include "sequence_scheduler.h"
#include <pb_decode.h>
#include <vector>

class SequenceDecoder {
  private:

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
  static bool decode_animation(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);

  public:
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
  static bool decode(pb_istream_t* stream, Sequence* sequence);
};
