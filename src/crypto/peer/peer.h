#include "sparkle/schwaemm.h"

#ifndef PEER_H
#define PEER_H

#define KEY_BYTES 16
#define NONCE_BYTES 32
#define MSG_LEN 32
#define CYPHER_LEN 48

typedef struct PeerState {
  UChar nonce[NONCE_BYTES];
  UChar key[KEY_BYTES];
} PeerState, * PeerStateP;

int sendMessage(PeerStateP thisPeer, UChar msg[32], UChar * cypher);
int receiveMessage(PeerStateP peer, UChar cypher[CYPHER_LEN], UChar * msg);
void initPeer(PeerStateP peer);

#endif // PEER_H