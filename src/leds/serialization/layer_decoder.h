#pragma once

#include "../layers/layer.h"
#include "debug.h"
#include "protocol.pb.h"
#include <pb_decode.h>
#include <vector>

class LayerDecoder {
  private:
  /**
   * This function reads a stream of varint-encoded color values and decodes them into
   * CRGB objects, which are then stored in the provided vector. The decoding process
   * continues until there are no more bytes left in the stream.
   *
   * @param stream A pointer to the input stream from which color values are read.
   * @param field A pointer to the field iterator (not used in this function).
   * @param arg A pointer to a vector of CRGB objects where the decoded colors will be stored.
   * @return true if all color values are successfully decoded and stored; false if an error occurs during decoding.
   */
  static bool decode_colors(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);

  /**
   * This function reads a stream of varint-encoded byte values and decodes them into
   * a vector of bytes. The decoding process continues until there are no more bytes
   * left in the stream.
   *
   * @param stream A pointer to the input stream from which byte values are read.
   * @param field A pointer to the field iterator (not used in this function).
   * @param arg A pointer to a vector of bytes where the decoded values will be stored.
   * @return true if all byte values are successfully decoded and stored; false if an error occurs during decoding.
   */
  static bool decode_bytes(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);

  public:
  /**
   * This function decodes a stream of varint-encoded layer data into a vector of ILayer objects.
   * The decoding process involves reading the stream, determining the type of layer being decoded,
   * and then decoding the appropriate data fields into the corresponding layer type.
   * Set args to a new std::vector<ILayer*>() to store the decoded layers.
   *
   * @param stream A pointer to the input stream from which layer data is read.
   * @param field A pointer to the field iterator (not used in this function).
   * @param arg A pointer to a vector of ILayer pointers where the decoded layers will be stored.
   * @return true if the layer is successfully decoded and stored; false if an error occurs during decoding.
   */
  static bool decode_layer(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);
};