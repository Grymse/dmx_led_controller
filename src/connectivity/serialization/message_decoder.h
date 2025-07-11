#pragma once

#include "common.h"

class MessageDecoder {
  private:

  static bool decode_message(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);

  public:
  static bool decode(pb_istream_t* stream);
};
