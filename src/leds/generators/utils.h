#pragma once
#include "Arduino.h"
#include <vector>

template <typename T, size_t N>
T pickOne(T (&arr)[N]) {
    return arr[random(N)];
}

template <typename T>
T pickOne(const std::vector<T>& vec) {
    return vec[rand() % vec.size()];
}