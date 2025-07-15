
#include "message_decoder.h"

/**
 * @brief Decodes a sequence from the provided protobuf stream.
 *
 * This static method is responsible for decoding a sequence from a protobuf stream
 * and populating the corresponding Sequence object. It sets up the necessary callback
 * for decoding animations within the sequence.
 *
 * @param stream The input stream from which the sequence data is read.
 * @param sequence A pointer to the Sequence object that will be populated with the decoded data.
 * @return true if the sequence is successfully decoded, false otherwise.
 */


 bool readTargetGroups(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
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


bool cb_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
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

      incoming_broadcast->target_groups.funcs.decode = readTargetGroups;
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

  incomingMessage.cb_payload.funcs.decode = cb_callback; // Set the callback for decoding the payload

  // Setup callbacks for decoding 
  if (!pb_decode(stream, protocol_Message_fields, &incomingMessage)) {
    debug("\033[1;31mFailed to decode message\033[0m\n", 0);
    return false;  // Return empty sequence if decoding fails
  }

  Sequence* sequence;

  switch (incomingMessage.which_payload) {
    case protocol_Message_sequence_tag: {
      sequence = static_cast<Sequence*>(incomingMessage.payload.sequence.animations.arg);
      // available through *sequence
      break;
    }
    case protocol_Message_broadcast_sequence_tag: {
      protocol_BroadcastSequence broadcast = incomingMessage.payload.broadcast_sequence;
      sequence = static_cast<Sequence*>(broadcast.sequence.animations.arg);
      std::vector<uint32_t>* group_ids = static_cast<std::vector<uint32_t>*>(broadcast.target_groups.arg);
      // available through *sequence
      // available through *group_ids
      break;
    }
    case protocol_Message_save_state_tag: {
      protocol_State* state = &incomingMessage.payload.save_state;
      sequence = static_cast<Sequence*>(state->sequence.animations.arg);

      // available through *sequence
      // available through state->settings
      break;
    }
    case protocol_Message_request_state_tag: {
      printf("\033[1;34mRequesting current state\033[0m\n", 0);
      // This is just a request, no sequence or settings to decode
      break;
    }
  }

  return true;
}