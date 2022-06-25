#include <stdio.h>
#include <string.h>
#include "peer/peer.h"
#ifdef DEBUGARDUINO
  #include "Arduino.h"
#endif

unsigned char key[32] = "000102030405060708090A0B0C0D0E0F";
unsigned char nonce[32] = "000102030405060708090A0B0C0D0E0F"; // Should probably be different than key

void incrementNonce(PeerStateP peer) {
  // FIXME: This implementation should sum whole array
  peer->nonce[0] += 1;
}

void initPeer(PeerStateP peer) {
  memcpy(peer->key, key, KEY_BYTES);
  memcpy(peer->nonce, nonce, NONCE_BYTES);
}

int encryptMessage(PeerStateP peer, UChar msg[32], UChar * cypher) {
  ULLInt cypherLen;
  int res = crypto_aead_encrypt(cypher, &cypherLen, msg, MSG_LEN, NULL, 0, NULL, peer->nonce, peer->key);
  if (res == 0) { // No Error, nonce should not be reused
    #ifdef DEBUG
      printf("RetornoCifra>>\n\tcod:%d\n\tlen:%llu\n\tcypher:", res, cypherLen);
      for (int i = 0; i < CYPHER_LEN; i ++) {
        printf("%02x", cypher[i]);
      }
      printf("\n");
    #endif
    incrementNonce(peer);
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

int decryptMessage(PeerStateP peer, UChar cypher[CYPHER_LEN], UChar * msg) {
  ULLInt msgLen;
  int res = crypto_aead_decrypt(msg, &msgLen, NULL, cypher, CYPHER_LEN, NULL, 0, peer->nonce, peer->key);
  if (res == 0) { // No Error, nonce should not be reused
    #ifdef DEBUG
      printf("RetornoDecode>>\n\tcod::%d\n\tlen::%llu\n\tmsg::%s\n", res, msgLen, msg);
    #endif
    incrementNonce(peer);
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

void init_buffer(UChar *buffer, ULLInt numbytes)
{
  ULLInt i;
  
  for (i = 0; i < numbytes; i++)
    buffer[i] = (UChar)i;
}

void test(int argc, char* argv[]) {
    UChar key[16] = {0};
    UChar nonce[32] = {0};
    UChar msg[32] = "Hello Crypto World";
    UChar msg2[32] = {0};
    UChar ad[32] = {0};
    UChar ct[32 + 16] = {0};
    ULLInt clen, mlen;
    ULLInt mlen2, adlen;

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