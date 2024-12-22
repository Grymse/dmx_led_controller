#include <pb_encode.h>
#include "./sequence_scheduler.h"
class SequenceEncoder {
private:
    static bool animations_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);
    
public:
    static bool encode(pb_ostream_t *stream, Sequence * sequence);
};
