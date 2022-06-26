#include "aead-common.h"
#include "xoodyak.h"

aead_cipher_encrypt_t crypto_aead_encrypt = xoodyak_aead_encrypt;
aead_cipher_decrypt_t crypto_aead_decrypt = xoodyak_aead_decrypt;