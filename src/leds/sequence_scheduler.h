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
  u8_t brightness;
  u16_t firstTick; // Which tick should the animation start on. Default is 0.
};

struct Sequence {
  std::vector<Animation*> animations;
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
  void add(std::vector<ILayer*> layers, u16_t tickDuration, Direction direction = Direction::FORWARD, u8_t brightness = 255, u16_t firstTick = 0);
  void add(Animation* animation);
  void set(std::vector<Animation*> animations);
  void set(Sequence* sequence);
  Sequence * getSequence();
  void clear();

  String getName() override;
  void update() override;
};;