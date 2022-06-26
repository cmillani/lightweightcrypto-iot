#include "sparkle/schwaemm.h"

#ifndef PEER_H
#define PEER_H

#define KEY_BYTES 16
#define NONCE_BYTES 32
#define MSG_LEN 32
#define CYPHER_LEN 48

typedef struct PeerState {
  UChar nonce_r[NONCE_BYTES];
  UChar nonce_w[NONCE_BYTES];
  UChar key[KEY_BYTES];
} PeerState, * PeerStateP;

int encryptMessage(PeerStateP thisPeer, UChar msg[32], UChar * cypher);
int decryptMessage(PeerStateP peer, UChar cypher[CYPHER_LEN], UChar * msg);
void initPeer(PeerStateP peer);

#endif // PEER_H