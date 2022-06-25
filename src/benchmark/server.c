#include <stdio.h>
#include <string.h>
#include "peer/peer.h"

int main(int argc, char* argv[]) {
  PeerState server;
  initPeer(&server);

  unsigned char cypher[CYPHER_LEN];
  unsigned char msg[MSG_LEN];

  for(;;) {
      int read = fread(cypher, sizeof(unsigned char), CYPHER_LEN, stdin);
      if (read == CYPHER_LEN) {
        decryptMessage(&server, cypher, msg);
      } else {
        break; // fread returned wrong size, stdin probably closed
      }
  }
  return 0;
}