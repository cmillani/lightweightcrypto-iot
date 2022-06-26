#include <stdio.h>
#include <string.h>
#include "peer/peer.h"
#ifdef DEBUGARDUINO
  #include "Arduino.h"
#endif

unsigned char key[32] = "000102030405060708090A0B0C0D0E0F";
unsigned char nonce[32] = "000102030405060708090A0B0C0D0E0F"; // Should probably be different than key

void incrementNonce(unsigned char * nonce) {
  // FIXME: This implementation should sum whole array
  nonce[0] += 1;
}

void initPeer(PeerStateP peer) {
  memcpy(peer->key, key, KEY_BYTES);
  memcpy(peer->nonce_w, nonce, NONCE_BYTES);
  memcpy(peer->nonce_r, nonce, NONCE_BYTES);
}

int encryptMessage(PeerStateP peer, unsigned char msg[32], unsigned char * cypher) {
  unsigned long long int cypherLen;
  int res = crypto_aead_encrypt(cypher, &cypherLen, msg, MSG_LEN, NULL, 0, NULL, peer->nonce_w, peer->key);
  if (res == 0) { // No Error, nonce should not be reused
    #ifdef DEBUG
      printf("RetornoCifra>>\n\tcod:%d\n\tlen:%llu\n\tcypher:", res, cypherLen);
      for (int i = 0; i < CYPHER_LEN; i ++) {
        printf("%02x", cypher[i]);
      }
      printf("\n");
    #endif
    incrementNonce(peer->nonce_w);
  } else {
    #ifdef DEBUGARDUINO
      Serial.print("ERROR: unable to encrypt, return code: ");
      Serial.println(res);
    #endif
    #ifdef DEBUG
      printf("ERROR: Invalid return %d\n", res);
    #endif
  }
  return res;
}

int decryptMessage(PeerStateP peer, unsigned char cypher[CYPHER_LEN], unsigned char * msg) {
  unsigned long long int msgLen;
  int res = crypto_aead_decrypt(msg, &msgLen, NULL, cypher, CYPHER_LEN, NULL, 0, peer->nonce_r, peer->key);
  if (res == 0) { // No Error, nonce should not be reused
    #ifdef DEBUG
      printf("RetornoDecode>>\n\tcod::%d\n\tlen::%llu\n\tmsg::%s\n", res, msgLen, msg);
    #endif
    incrementNonce(peer->nonce_r);
  } else {
    #ifdef DEBUGARDUINO
      Serial.print("ERROR: unable to decrypt, return code: ");
      Serial.println(res);
    #endif
    #ifdef DEBUG
      printf("ERROR: Decode invalid return %d\n", res);
      printf("RetornoDecode>>\n\tcod::%d\n\tlen::%llu\n\tmsg::%s\n", res, msgLen, msg);
    #endif
  }
  return res;
}

//////////// BENCH

void init_buffer(unsigned char *buffer, unsigned long long int numbytes)
{
  unsigned long long int i;
  
  for (i = 0; i < numbytes; i++)
    buffer[i] = (unsigned char)i;
}

void test(int argc, char* argv[]) {
    unsigned char key[16] = {0};
    unsigned char nonce[32] = {0};
    unsigned char msg[32] = "Hello Crypto World";
    unsigned char msg2[32] = {0};
    unsigned char ad[32] = {0};
    unsigned char ct[32 + 16] = {0};
    unsigned long long int clen, mlen;
    unsigned long long int mlen2, adlen;

    init_buffer(key, sizeof(key));
    init_buffer(nonce, sizeof(nonce));
    init_buffer(ad, sizeof(ad));

    mlen = sizeof(msg);
    adlen = sizeof(ad);

    printf("Antes: %s\n", msg);
    
    int func_ret = crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
    printf("Retorno: %d, %s\n", func_ret, ct);

    func_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key);
    printf("Retorno: %d, %s\n", func_ret, msg2);
}