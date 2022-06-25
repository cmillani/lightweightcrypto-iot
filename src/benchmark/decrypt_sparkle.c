#include <stdio.h>
#include <string.h>
#include "peer/peer.h"

int main(int argc, char* argv[]) {
  PeerState server;
  initPeer(&server);

  unsigned char cypher[CYPHER_LEN] = {0};
  unsigned char msg[MSG_LEN] = {0};

  for(;;) {
      int read = fread(cypher, sizeof(unsigned char), CYPHER_LEN, stdin);
      if (read == CYPHER_LEN) {
        memset(msg, 0, MSG_LEN); // Clears message first
        decryptMessage(&server, cypher, msg);
        int count = fwrite(msg, sizeof(unsigned char), MSG_LEN, stdout);
        if (count != MSG_LEN) {
          fprintf(stderr, "ERROR: Something prevented writing all data, ferror:%d", ferror(stdout));
          return 1;
        }
        fflush(stdout); // Sends this block immediatelly
      } else {
        break; // fread returned wrong size, stdin probably closed
      }
  }
  return 0;
}