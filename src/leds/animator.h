#pragma once

#include "state.h"
#include <vector>
#include "layers/layer.h"
#include "../scheduler/scheduler.h"

#define ANIMATION_DURATION_MAX 4320000 // 50 * 3600 * 24



class Animator : public Process {
  CRGB* leds;
  std::vector<ILayer*> layers;
  LEDState* state;
  u8_t brightness = 255;

  void resetTick();

  public:
  Animator(CRGB* leds, size_t size);
  void clear();
  void setBrightness(u8_t brightness);
  u8_t getBrightness();
  void setDirection(Direction direction);
  void setLayers(std::vector<ILayer*> layers);

  String getName();
  void update();
};