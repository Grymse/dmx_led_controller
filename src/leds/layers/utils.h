#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <vector>

class LayerUtils {
  public:
  static double mod(double a, double b);
  static String color_to_string(CRGB color);
  static String colors_to_string(std::vector<CRGB> colors);
  static String bytes_to_string(std::vector<u8_t> bytes);
};