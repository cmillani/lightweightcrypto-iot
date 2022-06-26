#if  defined(XOODYAK)
#include "xoodyak/wrapper.h"
#elif defined(GCM)
#include "gcm/lwc_crypto_aead.h"
#else
#include "sparkle/schwaemm.h"
#endif


#ifndef PEER_H
#define PEER_H

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_BYTES 16
#define NONCE_BYTES 32
#define MSG_LEN 32
#define CYPHER_LEN 48

typedef struct PeerState {
  unsigned char nonce_r[NONCE_BYTES];
  unsigned char nonce_w[NONCE_BYTES];
  unsigned char key[KEY_BYTES];
} PeerState, * PeerStateP;

int encryptMessage(PeerStateP thisPeer, unsigned char msg[32], unsigned char * cypher);
int decryptMessage(PeerStateP peer, unsigned char cypher[CYPHER_LEN], unsigned char * msg);
void initPeer(PeerStateP peer);

#ifdef __cplusplus
}
#endif

#endif // PEER_H