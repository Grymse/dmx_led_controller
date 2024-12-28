#ifndef ENCODER_H
#define ENCODER_H

#include <pb_encode.h>

class LayerEncoder {
private:
    static bool colors_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);
    static bool sections_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);

public:
    static bool layer_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);
};

#endif // ENCODER_H
