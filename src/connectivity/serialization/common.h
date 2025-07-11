#pragma once
#include "../../leds/serialization/sequence_decoder.h"
#include <pb_decode.h>
#include "protocol.pb.h"
#include "common.h"
#include <vector>

struct Settings {
  uint32_t group_id; // Group ID for this controller (used for selective message handling)
  uint32_t virtual_offset; // Virtual index of first LED for this controller
};

struct State {
  Sequence* sequence;
  Settings* settings;
};
