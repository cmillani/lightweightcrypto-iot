#ifndef EMPTY_H
#define EMPTY_H

int crypto_aead_encrypt(unsigned char *c, unsigned long long int *clen, const unsigned char *m, unsigned long long int mlen, \
  const unsigned char *ad, unsigned long long int adlen, const unsigned char *nsec, const unsigned char *npub, \
  const unsigned char *k);
int crypto_aead_decrypt(unsigned char *m, unsigned long long int *mlen, unsigned char *nsec, const unsigned char *c, \
  unsigned long long int clen, const unsigned char *ad, unsigned long long int adlen, const unsigned char *npub, \
  const unsigned char *k);

#endif // EMPTY_H