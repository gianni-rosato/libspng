#include <spng.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Do not use assert(): these checks must run in release builds too. */
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); \
    exit(1); \
} } while (0)

static void roundtrip(unsigned depth, unsigned color, unsigned channels)
{
    struct spng_ihdr header = {0};
    header.width = 17;
    header.height = 9;
    header.bit_depth = depth;
    header.color_type = color;
    const size_t size = header.width * header.height * channels * (depth / 8);
    uint16_t input[17 * 9 * 4], output[17 * 9 * 4];
    for (size_t i = 0; i < sizeof(input) / sizeof(*input); i++)
        input[i] = (uint16_t)(i * 7919 + 123);

    spng_ctx *encoder = spng_ctx_new(SPNG_CTX_ENCODER);
    CHECK(encoder != NULL);
    CHECK(spng_set_option(encoder, SPNG_ENCODE_TO_BUFFER, 1) == 0);
    CHECK(spng_set_ihdr(encoder, &header) == 0);
    CHECK(spng_encode_image(encoder, input, size, SPNG_FMT_PNG,
                           SPNG_ENCODE_FINALIZE) == 0);
    size_t png_size = 0;
    int error = -1;
    void *png = spng_get_png_buffer(encoder, &png_size, &error);
    CHECK(error == 0 && png != NULL && png_size > 0);
    spng_ctx_free(encoder);

    spng_ctx *decoder = spng_ctx_new(0);
    CHECK(decoder != NULL);
    CHECK(spng_set_png_buffer(decoder, png, png_size) == 0);
    struct spng_ihdr decoded;
    CHECK(spng_get_ihdr(decoder, &decoded) == 0);
    CHECK(decoded.width == header.width && decoded.height == header.height);
    CHECK(decoded.bit_depth == depth && decoded.color_type == color);
    size_t decoded_size = 0;
    CHECK(spng_decoded_image_size(decoder, SPNG_FMT_PNG, &decoded_size) == 0);
    CHECK(decoded_size == size);
    CHECK(spng_decode_image(decoder, output, size, SPNG_FMT_PNG, 0) == 0);
    CHECK(memcmp(input, output, size) == 0);
    spng_ctx_free(decoder);

    decoder = spng_ctx_new(0);
    CHECK(decoder != NULL);
    CHECK(spng_set_png_buffer(decoder, png, 8) == 0);
    CHECK(spng_get_ihdr(decoder, &decoded) != 0);
    spng_ctx_free(decoder);
    free(png);
}

int main(void)
{
    const unsigned colors[] = { SPNG_COLOR_TYPE_GRAYSCALE,
        SPNG_COLOR_TYPE_GRAYSCALE_ALPHA, SPNG_COLOR_TYPE_TRUECOLOR,
        SPNG_COLOR_TYPE_TRUECOLOR_ALPHA };
    const unsigned channels[] = {1, 2, 3, 4};
    for (unsigned i = 0; i < 4; i++) {
        roundtrip(8, colors[i], channels[i]);
        roundtrip(16, colors[i], channels[i]);
    }
    puts("8 PNG round trips and truncated-input checks passed");
    return 0;
}
