#include "layer_encoder.h"
#include "../layers/layer.h"
#include <vector>
#include "../layers/masks/masks.h"
#include "../layers/colors/colors.h"


u8_t calculate_encoding_size(u32_t value) {
    u8_t size = 0;
    do {
        value >>= 7;
        size++;
    } while (value > 0);

    return size;
}

bool pb_encode_size(pb_ostream_t *stream, u32_t* values, size_t values_size) {
    size_t size = 0;

    for (size_t i = 0; i < values_size; i++)
    {
        size += calculate_encoding_size(values[i]);
    }

    // Encode the length of the packed data
    if (!pb_encode_varint(stream, size)) {
        return false;
    }

    return true;
}

bool LayerEncoder::colors_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    if (!*arg) return true; // Nothing to encode

    std::vector<CRGB>* colors = static_cast<std::vector<CRGB>*>(*arg);
    u32_t* colors_int = new u32_t[colors->size()];

    for (int i = 0; i < colors->size(); i++)
    {
        CRGB color = colors->at(i);
        colors_int[i] = color.r << 16 | color.g << 8 | color.b;
    }

    if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
        return false;
    }

    if (!pb_encode_size(stream, colors_int, colors->size())) {
        return false;
    }
    

    for (size_t i = 0; i < colors->size(); i++)
    {

        if (!pb_encode_varint(stream, colors_int[i]))
        {
            printf("Encoding failed\n");
            return false;
        }
    }

    return true;
}

bool LayerEncoder::sections_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    if (!*arg) return true; // Nothing to encode

    std::vector<u8_t>* bytes = static_cast<std::vector<u8_t>*>(*arg);

    if (!pb_encode_tag(stream, PB_WT_STRING, field->tag) ||
        !pb_encode_varint(stream, bytes->size())) {
        return false;
    }
    
    for (u8_t byte : *bytes)
    {
        if (!pb_write(stream, &byte, 1))
        {
            printf("Encoding failed\n");
            return false;
        }
    }

    return true;
}

bool LayerEncoder::layer_callback(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{
    std::vector<ILayer*>* layers = static_cast<std::vector<ILayer*>*>(*arg);

    for (ILayer* layer : *layers)
    {
        protocol_Layer encoded_layer = layer->toEncodable();
        encoded_layer.colors.funcs.encode = colors_callback;
        encoded_layer.sections.funcs.encode = sections_callback;

        if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
            return false;
        }

        if (!pb_encode_submessage(stream, protocol_Layer_fields, &encoded_layer))
        {
            printf("Encoding failed\n");
            return false;
        }
    }

    return true;
}