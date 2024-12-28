#include "sequence_encoder.h"
#include "protocol.pb.h"
#include <vector>
#include "layer_encoder.h"
#include "../animator.h"


bool SequenceEncoder::animations_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    std::vector<Animation*>* animations = static_cast<std::vector<Animation*>*>(*arg);

    for (Animation* animation : *animations)
    {
        protocol_Direction direction = animation->direction == FORWARD ? protocol_Direction_FORWARD : protocol_Direction_BACKWARD;
        protocol_Animation encoded_animation = {
            .direction = direction,
            .duration = animation->tickDuration,
            .layers = {
                .funcs = {
                    .encode = LayerEncoder::layer_callback,
                },
                .arg = &animation->layers
            }
        };

        if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
            return false;
        }

        if (!pb_encode_submessage(stream, protocol_Animation_fields, &encoded_animation))
        {
            debug("\033[1;31mFailed to encode animation\033[0m\n", 0);
            return false;
        }
    }

    return true;
}

bool SequenceEncoder::encode(pb_ostream_t *stream, Sequence * sequence)
{
    protocol_Sequence encoded_sequence = {
        .brightness = sequence->brightness,
        .animations = {
            .funcs = {
                .encode = SequenceEncoder::animations_callback,
            },
            .arg = &sequence->animations
        }
    };

    if (!pb_encode(stream, protocol_Sequence_fields, &encoded_sequence))
        {
            debug("\033[1;31mFailed to encode sequence\033[0m\n", 0);
            return false;
        }
    
    return true;
}