#include <Arduino.h>
#include <FastLED.h>
#include <Animator.h>

class LightController {
public:
  CRGB leds[300];
  IAnimator* animator = nullptr;

  LightController(uint16_t NUM_LEDS, char DATA_PIN) {
    FastLED.addLeds<WS2812B, 10, GRB>(leds, NUM_LEDS);
  }

  void clearAnimation() {
    animator = nullptr;
  }

  void setAnimation(IAnimator* animator) {
    this->animator = animator;
  }
};