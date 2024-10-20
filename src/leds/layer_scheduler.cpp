#include <Arduino.h>
#include <vector>
#include "layers/layer.h"
#include "../scheduler/process.h"
#include "../debug.cpp"
#include "layer_controller.h"
#include "layer_scheduler.h"

/**
 * @brief A struct to represent an animation step
 *
 */
struct AnimationStep {
  std::vector<ILayer*> layers;
  u16_t tickDuration;
};
class LayerScheduler : public Process, public ILayerScheduler {
  u16_t currentStep = 0;
  u16_t tick = 0;
  std::vector<AnimationStep*> steps;
  ILayerController* controller;

  /**
   * @brief Reset the scheduler to the initial state
   *
   */
  void reset() {
    currentStep = 0;
    tick = 0;
  }

  public:
  /**
   * @brief Construct a new Layer Scheduler object
   *
   * @param controller The controller to use
   */
  LayerScheduler(ILayerController* controller) {
    this->controller = controller;
    clear();
  }

  String getName() override {
    return "Layer Scheduler";
  }

  /**
   * @brief Add a set of layers to the scheduler
   *
   * @param layers The layers to add
   * @param tickDuration The duration of the layers
   */
  void add(std::vector<ILayer*> layers, u16_t tickDuration) override {
    steps.push_back(new AnimationStep{ layers, tickDuration });
  }

  /**
   * @brief Clear the scheduler
   *
   */
  void clear() override {
    steps = {};
    controller->clear();
    reset();
  }

  /**
   * @brief A method to update the LED strip with the current animation
   * If the current step has exceeded its duration, the scheduler will move to the next step.
   * It should be called every 20ms
   */
  void update() override {
    if (steps.size() == 0) return;

    // Exceeded current steps duration => move to next step
    if (steps[currentStep]->tickDuration <= tick) {
      tick = 0;
      currentStep++;
    }

    // Erroneous state => reset;
    if (steps.size() <= currentStep) reset();

    // If first tick => apply layers. Cannot be added to statement above, as it would miss the first animation
    if (tick == 0) {
      controller->set(steps[currentStep]->layers);
    }

    tick++; // Progress
  }
};;