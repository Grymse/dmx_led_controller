#include "utils.h"
#include "math.h"

double LayerUtils::mod(double a, double b) {
  return a - b * floor(a / b);
}