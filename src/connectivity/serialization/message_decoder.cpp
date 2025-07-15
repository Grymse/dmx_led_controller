
#include "message_decoder.h"

bool message_decoder_readTargetGroups(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    std::vector<uint32_t>* group_ids = static_cast<std::vector<uint32_t>*>(*arg);
    while (stream->bytes_left)
    {
        uint32_t value;
        if (!pb_decode_varint32(stream, &value))
            return false;
        group_ids->push_back(value);
    }
    return true;
}

bool message_decoder_cb_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  if(field->tag == protocol_Message_sequence_tag) {
      protocol_Sequence *incoming_sequence = static_cast<protocol_Sequence*>(field->pData);
      Sequence* sequence = new Sequence();
      incoming_sequence->animations.funcs.decode = SequenceDecoder::decode_animation;
      incoming_sequence->animations.arg = sequence;

  } else if(field->tag == protocol_Message_broadcast_sequence_tag) {
      protocol_BroadcastSequence *incoming_broadcast = static_cast<protocol_BroadcastSequence*>(field->pData);
      Sequence* sequence = new Sequence();
      incoming_broadcast->sequence.animations.funcs.decode = SequenceDecoder::decode_animation;
      incoming_broadcast->sequence.animations.arg = sequence;

      incoming_broadcast->target_groups.funcs.decode = message_decoder_readTargetGroups;
      std::vector<uint32_t>* target_groups = new std::vector<uint32_t>();
      incoming_broadcast->target_groups.arg = target_groups;

  } else if (field->tag == protocol_Message_save_state_tag) {
      protocol_State *incoming_state = static_cast<protocol_State*>(field->pData);
      Sequence* sequence = new Sequence();
      incoming_state->sequence.animations.funcs.decode = SequenceDecoder::decode_animation;
      incoming_state->sequence.animations.arg = sequence;

  } else if (field->tag == protocol_Message_request_state_tag) {
    // No need for extra decoding, this is just a request
  }
  // Do not react on other messages.

  return true;
}

bool MessageDecoder::decode(pb_istream_t* stream) {
  protocol_Message incomingMessage = protocol_Message_init_zero;
  std::vector<uint32_t> target_groups;

  incomingMessage.cb_payload.funcs.decode = message_decoder_cb_callback; // Set the callback for decoding the payload

  // Setup callbacks for decoding 
  if (!pb_decode(stream, protocol_Message_fields, &incomingMessage)) {
    debug("\033[1;31mFailed to decode message\033[0m\n", 0);
    return false;  // Return empty sequence if decoding fails
  }

  Sequence* sequence;

  switch (incomingMessage.which_payload) {
    case protocol_Message_sequence_tag: {
      sequence = static_cast<Sequence*>(incomingMessage.payload.sequence.animations.arg);
      if (this->onSequenceReceived == nullptr) {
        debug("\033[1;31mNo callback set for sequence received\033[0m\n", 0);
        return false;
      }
      this->onSequenceReceived(sequence);
      break;
    }

    case protocol_Message_broadcast_sequence_tag: {
      protocol_BroadcastSequence broadcast = incomingMessage.payload.broadcast_sequence;
      sequence = static_cast<Sequence*>(broadcast.sequence.animations.arg);
      std::vector<uint32_t>* group_ids = static_cast<std::vector<uint32_t>*>(broadcast.target_groups.arg);
      if (this->onBroadcastSequenceReceived == nullptr) {
        debug("\033[1;31mNo callback set for broadcast sequence received\033[0m\n", 0);
        return false;
      }
      this->onBroadcastSequenceReceived(sequence, group_ids);
      break;
    }

    case protocol_Message_save_state_tag: {
      protocol_State* state = &incomingMessage.payload.save_state;
      sequence = static_cast<Sequence*>(state->sequence.animations.arg);

      if (this->onSaveStateReceived == nullptr) {
        debug("\033[1;31mNo callback set for save state received\033[0m\n", 0);
        return false;
      }
      this->onSaveStateReceived(sequence, &state->settings);
      break;
    }
    case protocol_Message_request_state_tag: {
      if (this->onRequestState == nullptr) {
        debug("\033[1;31mNo callback set for request state\033[0m\n", 0);
        return false;
      }
      this->onRequestState();
      break;
    }
  }

  return true;
}

void MessageDecoder::setOnSequenceReceived(OnSequenceReceived callback) {
  this->onSequenceReceived = callback;
}

void MessageDecoder::setOnBroadcastSequenceReceived(OnBroadcastSequenceReceived callback) {
  this->onBroadcastSequenceReceived = callback;
}

void MessageDecoder::setOnSaveStateReceived(OnSaveStateReceived callback) {
  this->onSaveStateReceived = callback;
}

void MessageDecoder::setOnRequestState(OnRequestState callback) {
  this->onRequestState = callback;
}