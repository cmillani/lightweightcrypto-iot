#include "aes.h"

void aes_key_schedule(const uint8_t *key, uint8_t *round_keys) {
    aes_key_schedule((uint8_t *) key, (uint8_t *) round_keys);
}