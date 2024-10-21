#include "layer.h"
#include "debug.h"
#include "protocol.pb.h"
#include <pb_decode.h>
#include <vector>
// Masks
#include <leds/layers/masks/blink.cpp>
#include <leds/layers/masks/invert.cpp>
#include <leds/layers/masks/pulse.cpp>
#include <leds/layers/masks/pulse_sawtooth.cpp>
#include <leds/layers/masks/sawtooth.cpp>
#include <leds/layers/masks/sections.cpp>
#include <leds/layers/masks/sections_wave.cpp>
#include <leds/layers/masks/stars.cpp>
#include <leds/layers/masks/wave.cpp>
// Colors
#include <leds/layers/colors/fade.cpp>
#include <leds/layers/colors/rainbow.cpp>
#include <leds/layers/colors/sections.cpp>
#include <leds/layers/colors/sections_wave.cpp>
#include <leds/layers/colors/single.cpp>
#include <leds/layers/colors/switch.cpp>

class LayerDecoder {
  private:
  std::vector<CRGB> static toCRGBs(std::vector<u32_t> input) {
    std::vector<CRGB> output;
    output.reserve(input.size());

    for (u32_t i : input) {
      output.push_back(CRGB(i));
    }

    return output;
  }

  std::vector<uint8_t> static toUint8s(const std::vector<uint32_t>& input) {
    std::vector<uint8_t> output;
    output.reserve(input.size());

    for (uint32_t value : input) {
      output.push_back(static_cast<uint8_t>(value));
    }

    return output;
  }

  public:
  /*
  static ILayer* decode(_protocol_Layer incomingLayer, std::vector<u32_t> effectSet) {
    switch (incomingLayer.type) {
      case protocol_LayerType_SingleColor:
        return new SingleColor(CRGB(effectSet[0]));

      case protocol_LayerType_RainbowColor:
        return new RainbowColor(incomingLayer.p1, incomingLayer.p2);

      case protocol_LayerType_Sections_waveColor:
        return new SectionsWaveColor(toCRGBs(effectSet), incomingLayer.p1);

      case protocol_LayerType_SectionsColor:
        return new SectionsColor(toCRGBs(effectSet), incomingLayer.p1);

      case protocol_LayerType_FadeColor:
        return new FadeColor(toCRGBs(effectSet), incomingLayer.p1);

      case protocol_LayerType_SwitchColor:
        return new SwitchColor(toCRGBs(effectSet), incomingLayer.p1);

      case protocol_LayerType_BlinkMask:
        return new BlinkMask(toUint8s(effectSet), incomingLayer.p1);

      case protocol_LayerType_InvertMask:
        return new InvertMask();

      case protocol_LayerType_Pulse_sawtoothMask:
        return new PulseSawtoothMask(incomingLayer.p1, incomingLayer.p2);

      case protocol_LayerType_PulseMask:
        return new PulseMask(incomingLayer.p1, incomingLayer.p2);

      case protocol_LayerType_SawtoothMask:
        return new SawtoothMask(incomingLayer.p1, incomingLayer.p2, incomingLayer.p3);

      case protocol_LayerType_Sections_waveMask:
        return new SectionsWaveMask(toUint8s(effectSet), incomingLayer.p1);

      case protocol_LayerType_SectionsMask:
        return new SectionsMask(toUint8s(effectSet), incomingLayer.p1);

      case protocol_LayerType_StarsMask:
        return new StarsMask(incomingLayer.p1, incomingLayer.p2, incomingLayer.p3);

      case protocol_LayerType_WaveMask:
        return new WaveMask(incomingLayer.p1, incomingLayer.p2, incomingLayer.p3);
    }

    debug("Missing layerId %d", incomingLayer.type);
    return nullptr;
  } */

  static bool decode_colors(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
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

  static bool decode_bytes(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    std::vector<u8_t>* bytes = static_cast<std::vector<u8_t>*>(*arg);

    while (stream->bytes_left) {
      uint32_t value;
      if (!pb_decode_varint32(stream, &value)) {
        debug("\033[1;31mFailed to decode colors\033[0m\n", 0);
        return false;
      }
      bytes->push_back(value);
    }
    return true;
  }

  bool decode_layer(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
    protocol_Layer incomingLayer = protocol_Layer_init_zero;
    ILayer* layer;
    std::vector<CRGB> colors;
    std::vector<u8_t> bytes;

    // For any field that is a repeated field, we need to create a vector to store the values
    incomingLayer.payload.fadeColor.colors.arg = &colors;
    incomingLayer.payload.fadeColor.colors.funcs.decode = decode_colors;
    incomingLayer.payload.sectionsWaveColor.sections.arg = &colors;
    incomingLayer.payload.sectionsWaveColor.sections.funcs.decode = decode_colors;
    incomingLayer.payload.sectionsColor.sections.arg = &colors;
    incomingLayer.payload.sectionsColor.sections.funcs.decode = decode_colors;
    incomingLayer.payload.switchColor.colors.arg = &colors;
    incomingLayer.payload.switchColor.colors.funcs.decode = decode_colors;

    incomingLayer.payload.blinkMask.pattern.arg = &bytes;
    incomingLayer.payload.blinkMask.pattern.funcs.decode = decode_bytes;
    incomingLayer.payload.sectionsWaveMask.sections.arg = &bytes;
    incomingLayer.payload.sectionsWaveMask.sections.funcs.decode = decode_bytes;
    incomingLayer.payload.sectionsMask.sections.arg = &bytes;
    incomingLayer.payload.sectionsMask.sections.funcs.decode = decode_bytes;


    if (!pb_decode(stream, protocol_Layer_fields, &incomingLayer)) {
      debug("\033[1;31mFailed to decode sequence\033[0m\n", 0);
      return false;  // Return empty sequence if decoding fails
    }

    switch (incomingLayer.which_payload) {
      case protocol_Layer_fadeColor_tag:
        layer = new FadeColor(colors, incomingLayer.payload.fadeColor.duration);
        break;
      case protocol_Layer_rainbowColor_tag:
        layer = new RainbowColor(incomingLayer.payload.rainbowColor.duration, incomingLayer.payload.rainbowColor.length);
        break;
      case protocol_Layer_sectionsWaveColor_tag:
        layer = new SectionsWaveColor(colors, incomingLayer.payload.sectionsWaveColor.duration);
        break;
      case protocol_Layer_sectionsColor_tag:
        layer = new SectionsColor(colors, incomingLayer.payload.sectionsColor.duration);
        break;
      case protocol_Layer_singleColor_tag:
        layer = new SingleColor(CRGB(incomingLayer.payload.singleColor.color));
        break;
      case protocol_Layer_switchColor_tag:
        layer = new SwitchColor(colors, incomingLayer.payload.switchColor.duration);
        break;
      case protocol_Layer_blinkMask_tag:
        layer = new BlinkMask(bytes, incomingLayer.payload.blinkMask.duration);
        break;
      case protocol_Layer_invertMask_tag:
        layer = new InvertMask();
        break;
      case protocol_Layer_pulseSawtoothMask_tag:
        layer = new PulseSawtoothMask(incomingLayer.payload.pulseSawtoothMask.pulse_gap, incomingLayer.payload.pulseSawtoothMask.duration);
        break;
      case protocol_Layer_pulseMask_tag:
        layer = new PulseMask(incomingLayer.payload.pulseMask.pulse_gap, incomingLayer.payload.pulseMask.duration);
        break;
      case protocol_Layer_sawtoothMask_tag:
        layer = new SawtoothMask(incomingLayer.payload.sawtoothMask.wavelength, incomingLayer.payload.sawtoothMask.wavegap, incomingLayer.payload.sawtoothMask.duration);
        break;
      case protocol_Layer_sectionsWaveMask_tag:
        layer = new SectionsWaveMask(bytes, incomingLayer.payload.sectionsWaveMask.duration);
        break;
      case protocol_Layer_sectionsMask_tag:
        layer = new SectionsMask(bytes, incomingLayer.payload.sectionsMask.duration);
        break;
      case protocol_Layer_starsMask_tag:
        layer = new StarsMask(incomingLayer.payload.starsMask.frequency, incomingLayer.payload.starsMask.decaySpeed, incomingLayer.payload.starsMask.starLength);
        break;
      case protocol_Layer_waveMask_tag:
        layer = new WaveMask(incomingLayer.payload.waveMask.wavelength, incomingLayer.payload.waveMask.wavegap, incomingLayer.payload.waveMask.duration);
        break;
      default:
        debug("Missing layerId %d", incomingLayer.which_payload);
        return false;
    }

    static_cast<std::vector<ILayer*>*>(*arg)->push_back(layer);
    return true;
  }
};