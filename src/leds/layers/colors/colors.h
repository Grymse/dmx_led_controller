#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <colorutils.h>
#include <vector>
#include "../layer.h"

class FadeColor : public ILayer {
  u16_t duration;
  std::vector<CRGB> colors;

  public:
  FadeColor(std::vector<CRGB> colors, u16_t duration);
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state) override;
};


class RainbowColor : public ILayer {
  float duration;
  float length;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  RainbowColor(u16_t duration, u16_t length);
  CRGB apply(CRGB color, LEDState* state) override;
};


class SectionsWaveColor : public ILayer {
  u16_t duration;
  std::vector<CRGB> colors;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SectionsWaveColor(std::vector<CRGB> colors, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};


class SectionsColor : public ILayer {
  u16_t duration;

  public:
  std::vector<CRGB> colors;
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SectionsColor(std::vector<CRGB> colors, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};


class SingleColor : public ILayer {
  CRGB localColor;

  public:
  String getName();
  SingleColor(CRGB color);
  void setColor(CRGB color);
  String toString() override;
  protocol_Layer toEncodable() override;
  CRGB apply(CRGB color, LEDState* state);
};


class SwitchColor : public ILayer {
  std::vector<CRGB> colors;
  u16_t duration;

  public:
  String getName() override;
  String toString() override;
  protocol_Layer toEncodable() override;
  SwitchColor(std::vector<CRGB> colors, u16_t duration);
  CRGB apply(CRGB color, LEDState* state) override;
};