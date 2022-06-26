#ifndef CRYPTO_WRAPPER_H
#define CRYPTO_WRAPPER_H

#include "aead-common.h"
#include "xoodyak.h"

extern aead_cipher_encrypt_t crypto_aead_encrypt;
extern aead_cipher_decrypt_t crypto_aead_decrypt;
#endif // CRYPTO_WRAPPER_H