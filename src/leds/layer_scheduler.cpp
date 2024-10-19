#include <Arduino.h>
#include <vector>
#include "layers/layer.h"
#include "../scheduler/process.h"
#include "../debug.cpp"
#include "layer_controller.h"
#include "layer_scheduler.h"

struct AnimationStep {
  std::vector<ILayer*> pattern;
  u16_t tickDuration;
};

class LayerScheduler : public Process, public ILayerScheduler {
  u16_t currentStep = 0;
  u16_t tick = 0;
  std::vector<AnimationStep*> steps;
  ILayerController* controller;

  void reset() {
    currentStep = 0;
    tick = 0;
  }

public:
  LayerScheduler(ILayerController* controller) {
    this->controller = controller;
    clear();
  }

  void add(std::vector<ILayer*> pattern, u16_t tickDuration) override {
    steps.push_back(new AnimationStep{pattern, tickDuration});
  }

  void clear() override {
    steps = {};
    controller->clear();
    reset();
  }

  /**
   * @brief A method to update the LED strip with the current pattern
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

    // If first tick => apply pattern. Cannot be added to statement above, as it would miss the first animation
    if (tick == 0) {
      controller->set(steps[currentStep]->pattern);
    }

    tick++; // Progress
  }
};;