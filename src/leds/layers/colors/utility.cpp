#include <Arduino.h>
#include <FastLED.h>

class ColorUtility {
  public:

  /**
   * @brief Fades from one color to another based on a percentage.
   * @param from The color to fade from.
   * @param to The color to fade to.
   * @param percent The percentage of the fade.
   * @return The color after the fade.
   */
  static CRGB FadeFromTo(CRGB from, CRGB to, float percent) {
    CRGB from2 = from.scale8(255 - percent * 255);
    CRGB to2 = to.scale8(percent * 255);
    return from2 + to2;
  }
};