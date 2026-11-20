/* Buffer adapter for the Melodica tuner protocol. The caller owns the buffers. */
#include "blowfish.h"

#if defined(_WIN32)
#define WIRE_API __declspec(dllexport)
#else
#define WIRE_API __attribute__((visibility("default")))
#endif

WIRE_API int melodica_wire_blocks(uint8_t *bytes, size_t length, const uint8_t *key,
                                  size_t key_length, int decrypt) {
    MelodicaBlowfish context;
    if ((length && !bytes) || length % 8 || !melodica_blowfish_init(&context, key, key_length)) {
        return 0;
    }
    for (size_t offset = 0; offset < length; offset += 8) {
        if (decrypt) melodica_blowfish_decrypt_block(&context, bytes + offset);
        else melodica_blowfish_encrypt_block(&context, bytes + offset);
    }
    melodica_blowfish_clear(&context);
    return 1;
}
