#include <Arduino.h>
#include <vector>
#include "layers/layer.h"
#include "../scheduler/process.h"
#include "debug.h"
#include "animator.h"
#include "sequence_scheduler.h"

class SequenceScheduler : public Process, public ISequenceScheduler {
  u16_t currentAnimation = 0;
  u16_t tick = 0;
  std::vector<Animation*> animations;
  IAnimator* controller;

  /**
   * @brief Reset the scheduler to the initial state
   *
   */
  void reset() {
    currentAnimation = 0;
    tick = 0;
  }

  public:
  /**
   * @brief Construct a new Layer Scheduler object
   *
   * @param controller The controller to use
   */
  SequenceScheduler(IAnimator* controller) {
    this->controller = controller;
    clear();
  }

  String getName() override {
    return "Sequence Scheduler";
  }

  /**
   * @brief Add an animation to the scheduler
   *
   * @param layers The layers to add
   * @param tickDuration The duration of the layers
   * @param direction The direction of the animation
   */
  void add(std::vector<ILayer*> layers, u16_t tickDuration, Direction direction = Direction::FORWARD) override {
    this->add(new Animation{ layers, tickDuration, direction });
  }

  /**
   * @brief Add an animation to the scheduler
   *
   * @param animation The animation to add
   */
  virtual void add(Animation* animation) override {
    animations.push_back(animation);
  }

  /**
   * @brief Set sequence of animations in the scheduler
   *
   * @param animations The new sequence of animations
   * @param brightness The brightness of the sequence (Optional)
   */
  void set(std::vector<Animation*> animations, u8_t brightness = 255) {
    set(new Sequence({ animations, brightness }));
  }

  /**
   * @brief Set sequence of animations in the scheduler
   *
   * @param sequence The new sequence of animations
   */
  void set(Sequence* sequence) {
    clear();
    animations = sequence->animations;
    setBrightness(sequence->brightness);
  }

  /**
   * @brief Set the Brightness of the LED strip
   *
   * @param brightness new brightness 0-255
   */
  void setBrightness(u8_t brightness) {
    controller->setBrightness(brightness);
  }

  /**
   * @brief Clear the scheduler
   *
   */
  void clear() override {
    animations = {};
    setBrightness(255);
    controller->clear();
    reset();
  }

  /**
   * @brief A method to update the LED strip with the current animation
   * If the current step has exceeded its duration, the scheduler will move to the next step.
   * It should be called every 20ms
   */
  void update() override {
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
      controller->setLayers(animations[currentAnimation]->layers);
      controller->setDirection(animations[currentAnimation]->direction);
    }

    tick++; // Progress
  }
};;