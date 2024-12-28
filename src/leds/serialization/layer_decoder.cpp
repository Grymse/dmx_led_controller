#include "../layers/layer.h"
#include "debug.h"
#include "protocol.pb.h"
#include <pb_decode.h>
#include <vector>
#include "layer_decoder.h"
#include "../layers/masks/masks.h"
#include "../layers/colors/colors.h"

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
bool LayerDecoder::decode_colors(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
  std::vector<CRGB>* colors = static_cast<std::vector<CRGB>*>(*arg);

  while (stream->bytes_left) {
    uint32_t value;
    if (!pb_decode_varint32(stream, &value)) {
      debug("\033[1;31mFailed to decode colors\033[0m\n", 0);
      return false;
    }
    colors->push_back(CRGB(value));
  }

  return true;
}

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
bool LayerDecoder::decode_bytes(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
  std::vector<u8_t>* bytes = static_cast<std::vector<u8_t>*>(*arg);

  while (stream->bytes_left) {
    u8_t value;
    if (!pb_read(stream, &value, 1)) {
      debug("\033[1;31mFailed to decode bytes\033[0m\n", 0);
      return false;
    }
    bytes->push_back(value);
  }

  return true;
}

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
bool LayerDecoder::decode_layer(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
  std::vector<ILayer*>* layers = static_cast<std::vector<ILayer*>*>(*arg);
  protocol_Layer incomingLayer = protocol_Layer_init_zero; // Empty layer to store incoming data in.
  ILayer* layer; // Empty pointer to store layer in.

  std::vector<CRGB> colors;
  std::vector<u8_t> bytes;

  incomingLayer.sections.funcs.decode = LayerDecoder::decode_bytes;
  incomingLayer.sections.arg = &bytes;
  incomingLayer.colors.funcs.decode = LayerDecoder::decode_colors;
  incomingLayer.colors.arg = &colors;

  // Perform the decoding of the incomingLayer
  if (!pb_decode(stream, protocol_Layer_fields, &incomingLayer)) {
    debug("\033[1;31mFailed to decode sequence\033[0m\n", 0);
    return false;  // Return empty sequence if decoding fails
  }

  // Large switch statement to determine which layer type is being decoded
  switch (incomingLayer.type) {
    case protocol_LayerType_FadeColor:
      layer = new FadeColor(colors, incomingLayer.duration);
      break;
    case protocol_LayerType_RainbowColor:
      layer = new RainbowColor(incomingLayer.duration, incomingLayer.length);
      break;
    case protocol_LayerType_SectionsWaveColor:
      layer = new SectionsWaveColor(colors, incomingLayer.duration);
      break;
    case protocol_LayerType_SectionsColor:
      layer = new SectionsColor(colors, incomingLayer.duration);
      break;
    case protocol_LayerType_SingleColor:
      layer = new SingleColor(CRGB(incomingLayer.color));
      break;
    case protocol_LayerType_SwitchColor:
      layer = new SwitchColor(colors, incomingLayer.duration);
      break;
    
    case protocol_LayerType_BlinkMask:
      layer = new BlinkMask(bytes, incomingLayer.duration);
      break;
    case protocol_LayerType_InvertMask:
      layer = new InvertMask();
      break;
    case protocol_LayerType_PulseSawtoothMask:
      layer = new PulseSawtoothMask(incomingLayer.gap, incomingLayer.duration);
      break;
    case protocol_LayerType_PulseMask:
      layer = new PulseMask(incomingLayer.gap, incomingLayer.duration);
      break;
    case protocol_LayerType_SawtoothMask:
      layer = new SawtoothMask(incomingLayer.length, incomingLayer.gap, incomingLayer.duration);
      break;
    case protocol_LayerType_SectionsWaveMask:
      layer = new SectionsWaveMask(bytes, incomingLayer.duration);
      break;
    case protocol_LayerType_SectionsMask:
      layer = new SectionsMask(bytes, incomingLayer.duration);
      break;
    case protocol_LayerType_StarsMask:
      layer = new StarsMask(incomingLayer.frequency, incomingLayer.speed, incomingLayer.length);
      break;
    case protocol_LayerType_WaveMask:
      layer = new WaveMask(incomingLayer.length, incomingLayer.gap, incomingLayer.duration);
      break;
    default:
      debug("Missing layer type %d", incomingLayer.type);
      return false;
  }

  // Push the decoded layer into the layers vector
  layers->push_back(layer);
  return true;
}