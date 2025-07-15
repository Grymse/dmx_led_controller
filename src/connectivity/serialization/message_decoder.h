#pragma once

#include "common.h"

typedef void (*OnSequenceReceived)(Sequence* sequence);
typedef void (*OnBroadcastSequenceReceived)(Sequence* sequence, std::vector<uint32_t>* group_ids);
typedef void (*OnSaveStateReceived)(Sequence* sequence, protocol_Settings* settings);
typedef void (*OnRequestState)();

class MessageDecoder {
  OnSequenceReceived onSequenceReceived = nullptr;
  OnBroadcastSequenceReceived onBroadcastSequenceReceived = nullptr;
  OnSaveStateReceived onSaveStateReceived = nullptr;
  OnRequestState onRequestState = nullptr;

  public:
  bool decode(pb_istream_t* stream);
  void setOnSequenceReceived(OnSequenceReceived callback);
  void setOnBroadcastSequenceReceived(OnBroadcastSequenceReceived callback);
  void setOnSaveStateReceived(OnSaveStateReceived callback);
  void setOnRequestState(OnRequestState callback);
};