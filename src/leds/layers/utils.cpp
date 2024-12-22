#include "utils.h"
#include "math.h"
#include <sstream>
#include <iomanip>

double LayerUtils::mod(double a, double b) {
  return a - b * floor(a / b);
}
String LayerUtils::color_to_string(CRGB color) {
  char buffer[8];
  sprintf(buffer, "#%02X%02X%02X", color.r, color.g, color.b);
  return String(buffer);
}

String LayerUtils::colors_to_string(std::vector<CRGB> colors) {
  std::ostringstream result;
  result << "[";
  for (size_t i = 0; i < colors.size(); ++i) {
    result << color_to_string(colors[i]).c_str();
    if (i < colors.size() - 1) {
      result << ", ";
    }
  }
  result << "]";
  return String(result.str().c_str());
}
String LayerUtils::bytes_to_string(std::vector<u8_t> bytes) {
  std::ostringstream result;
  result << "[";
  for (size_t i = 0; i < bytes.size(); ++i) {
    result << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    if (i < bytes.size() - 1) {
      result << ", ";
    }
  }
  result << "]";
  return String(result.str().c_str());
}
