#include <Arduino.h>
#ifndef LEDSTATE_H
#define LEDSTATE_H

struct LEDState
{
  long tick; // Current tick of animation
  u16_t index; // Current index of the LED strip
  size_t length; // Length of the LED strip
  // u16_t offset; // offset index, such that this device comes after another device. E.g. Decide 1: 0-299 (offset 0), Device 2: 300-599 (offset 300)
};
#endif