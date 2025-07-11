
#include "message_decoder.h"

/* 
message Sequence {
  repeated Animation animations = 2;
}

enum MessageType {
  SET_SEQUENCE = 0; // For live setting the current animation sequence.
  SAVE_SEQUENCE = 1; // Save new default sequence to the device.
  SAVE_SETTINGS = 2; // Save new settings to the device.
  REQUEST_STATE = 3; // Request the current sequence and settings from the device.
  RESPONSE_STATE = 4; // Response with the current sequence and settings from the device.
}

enum PropagationMode {
  LOCAL_ONLY = 0;     // Execute locally only, do not broadcast via antenna
  BROADCAST_ALL = 1;  // Broadcast via antenna to all controllers (all will execute)
  BROADCAST_GROUPS = 2; // Broadcast via antenna, only specified groups will execute
}

message Settings {
  uint32 group_id = 1; // Group ID for this controller (used for selective message handling)
  uint32 virtual_offset = 2; // Virtual index of first LED for this controller
}

message Message {
  MessageType type = 1;
  
  // Content fields (mutually exclusive based on message type)
  Settings settings = 2; // Present for SAVE_SETTINGS messages
  Sequence sequence = 3; // Present for SET_SEQUENCE and SAVE_SEQUENCE messages
  
  // Propagation control (only relevant for messages received via Bluetooth)
  PropagationMode propagation = 4; // How the gateway controller should propagate this message
  repeated uint32 target_groups = 5; // Groups to execute when propagation = BROADCAST_GROUPS
  
  // Note: Controllers receiving messages via antenna will:
  // - Always execute the message if propagation = BROADCAST_ALL
  // - Execute only if their group_id is in target_groups when propagation = BROADCAST_GROUPS
  // - Never see messages with propagation = LOCAL_ONLY (not propagated)
} */

/* 

bool SequenceDecoder::decode_animation(pb_istream_t* stream, const pb_field_iter_t* field, void** arg) {
  _protocol_Animation incomingAnimation = protocol_Animation_init_zero;
  Animation* animation = new Animation();
  Sequence* sequence = static_cast<Sequence*>(*arg);
  sequence->animations.push_back(animation);

  std::vector<ILayer*>* layers = new std::vector<ILayer*>();
  incomingAnimation.layers.funcs.decode = LayerDecoder::decode_layer;
  incomingAnimation.layers.arg = &animation->layers;

  // Decode the incomingAnimation from the stream
  if (!pb_decode(stream, protocol_Animation_fields, &incomingAnimation)) {
    debug("\033[1;31mFailed to decode animation\033[0m\n", 0);
    return false;  // Return false if decoding fails
  }

  animation->direction = incomingAnimation.direction == protocol_Direction_FORWARD ? Direction::FORWARD : Direction::BACKWARD;
  animation->tickDuration = incomingAnimation.duration;
  animation->firstTick = incomingAnimation.first_tick;
  animation->brightness = incomingAnimation.brightness;

  return true;  // Return true if decoding is successful
}

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
  std::vector<uint32_t> target_groups;

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