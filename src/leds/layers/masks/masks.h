#pragma once

#include <Arduino.h>
#include <vector>
#include "../layer.h"

class BlinkMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> pattern;

  public:
  BlinkMask(std::vector<u8_t> pattern, u16_t duration);
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state) override;
};

class InvertMask : public ILayer {
  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state) override;
};

class PulseSawtoothMask : public ILayer {
  u16_t duration;
  u16_t pulse_gap;

  public:
  PulseSawtoothMask(u16_t pulse_gap, u16_t duration);
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state) override;
};

class SectionsRandomMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> sections;
  u8_t current_section;
  u32_t tick_of_next_update;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SectionsRandomMask(std::vector<u8_t> sections, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};

class PulseMask : public ILayer {
  u16_t duration;
  u16_t pulse_gap;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  PulseMask(u16_t pulse_gap, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};

class SawtoothMask : public ILayer {
  u16_t wavelength;
  u16_t duration;
  u16_t wavegap;

  public:
  SawtoothMask(u16_t wavelength, u16_t wavegap, u16_t duration);
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state) override;
};

class SectionsWaveMask : public ILayer {
  u16_t duration;
  std::vector<u8_t> sections;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SectionsWaveMask(std::vector<u8_t> sections, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};

class SectionsMask : public ILayer {
  u16_t duration;

  public:
  std::vector<u8_t> sections;
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SectionsMask(std::vector<u8_t> sections, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};

class StarsMask : public ILayer {
  u16_t frequency;
  u8_t decaySpeed;
  u8_t starLength;
  std::vector<u8_t> multipliers = {};

  u8_t decay(u8_t multiplier);
  void adjustVector(size_t length);
  void brightenNeighbourLEDs(LEDState* state);

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  StarsMask(u16_t frequency, u8_t decaySpeed, u8_t starLength);
  CRGB apply(CRGB color, LEDState* state) override;
};

class WaveMask : public ILayer {
  u16_t wavelength;
  u16_t duration;
  u16_t wavegap;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  WaveMask(u16_t wavelength, u16_t wavegap, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};