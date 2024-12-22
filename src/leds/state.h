
#pragma once
#include <Arduino.h>

enum Direction { FORWARD = 1, BACKWARD = -1 };

/**
 * @brief The state of an LED strip, including the current tick and index.
 */
struct LEDState {
  long tick; // Current tick of animation
  u16_t index; // Current index of the LED strip
  size_t length; // Length of the LED strip
  Direction direction; // Direction of the animation
  // u16_t offset; // offset index, such that this device comes after another device. E.g. Decide 1: 0-299 (offset 0), Device 2: 300-599 (offset 300)
};