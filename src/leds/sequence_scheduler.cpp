#include <Arduino.h>
#include <vector>
#include "sequence_scheduler.h"

/**
 * @brief Reset the scheduler to the initial state
 *
 */
void SequenceScheduler::reset() {
  currentAnimation = 0;
  tick = 0;
}

/**
 * @brief Construct a new Layer Scheduler object
 *
 * @param animator The animator to use
 */
SequenceScheduler::SequenceScheduler(Animator* animator) {
  this->animator = animator;
  clear();
}

String SequenceScheduler::getName() {
  return "Sequence Scheduler";
}

/**
 * @brief Add an animation to the scheduler
 *
 * @param layers The layers to add
 * @param tickDuration The duration of the layers
 * @param direction The direction of the animation
 */

void SequenceScheduler::add(std::vector<ILayer*> layers, u16_t tickDuration, Direction direction, u8_t brightness, u16_t firstTick) {
  this->add(new Animation{ layers, tickDuration, direction, brightness, firstTick });
}

/**
 * @brief Add an animation to the scheduler
 *
 * @param animation The animation to add
 */
void SequenceScheduler::add(Animation* animation) {
  animation->tickDuration == 0 ? ANIMATION_DURATION_MAX : animation->tickDuration; // Set duration to max if not set
  animations.push_back(animation);
}

/**
 * @brief Set sequence of animations in the scheduler
 * @param animations The new sequence of animations
 */
void SequenceScheduler::set(std::vector<Animation*> animations) {
  set(new Sequence({ animations }));
}

/**
 * @brief Set sequence of animations in the scheduler
 *
 * @param sequence The new sequence of animations
 */
void SequenceScheduler::set(Sequence* sequence) {
  clear();
  animations = sequence->animations;
  for (Animation* animation : animations) {
    animation->tickDuration = animation->tickDuration == 0 ? ANIMATION_DURATION_MAX : animation->tickDuration; // Set duration to max if not set
  }
}


/**
 * @brief Clear the scheduler
 *
 */
void SequenceScheduler::clear() {
  animations = {};
  animator->clear();
  reset();
}

/**
 * @brief Get the current sequence
 *
 * @return Sequence* The current sequence
 */
Sequence* SequenceScheduler::getSequence() {
  return new Sequence({ animations });
}

/**
 * @brief A method to update the LED strip with the current animation
 * If the current step has exceeded its duration, the scheduler will move to the next step.
 * It should be called every 20ms
 */
void SequenceScheduler::update() {
  if (animations.size() == 0) return;

  // Exceeded current steps duration => move to next step
  if (animations[currentAnimation]->tickDuration <= tick) {
    tick = 0;
    currentAnimation++;
  }

  // Erroneous state => reset;
  if (animations.size() <= currentAnimation) reset();

  // If first tick => apply pattern. Cannot be added to statement above, as it would miss the first animation
  if (tick == 0) {
    animator->setLayers(animations[currentAnimation]->layers);
    animator->setDirection(animations[currentAnimation]->direction);
    animator->setBrightness(animations[currentAnimation]->brightness);
    animator->setTick(animations[currentAnimation]->firstTick);
  }

  tick++; // Progress
}