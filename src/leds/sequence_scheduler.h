#pragma once

#include <vector>
#include "layers/layer.h"
#include "../scheduler/scheduler.h"
#include "debug.h"
#include "animator.h"
#include <Arduino.h>

struct Animation {
  std::vector<ILayer*> layers;
  u16_t tickDuration;
  Direction direction;
};

struct Sequence {
  std::vector<Animation*> animations;
  u8_t brightness;
};


class SequenceScheduler : public Process {
  u16_t currentAnimation = 0;
  u16_t tick = 0;
  std::vector<Animation*> animations;
  Animator* animator;

  /**
   * @brief Reset the scheduler to the initial state
   *
   */
  void reset();

  public:
  SequenceScheduler(Animator* animator);
  void add(std::vector<ILayer*> layers, u16_t tickDuration, Direction direction = Direction::FORWARD);
  void add(Animation* animation);
  void set(std::vector<Animation*> animations, u8_t brightness = 255);
  void set(Sequence* sequence);
  Sequence * getSequence();
  void setBrightness(u8_t brightness);
  void clear();

  String getName() override;
  void update() override;
};;