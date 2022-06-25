#include <stdio.h>
#include <string.h>
#include "peer/peer.h"

int main(int argc, char* argv[]) {
  PeerState client;
  initPeer(&client);

  unsigned char msg[MSG_LEN] = {0};
  unsigned char cypher[CYPHER_LEN] = {0};

  for(;;) {
      char * ret = fgets((char *) msg, sizeof(msg), stdin);
      if (ret != NULL) {
        encryptMessage(&client, msg, cypher);
        int count = fwrite(cypher, sizeof(unsigned char), CYPHER_LEN, stdout);
        if (count != CYPHER_LEN) {
          fprintf(stderr, "ERROR: Something prevented writing all data, ferror:%d", ferror(stdout));
          return 1;
        }
        fflush(stdout); // Sends this block immediatelly
      } else {
        break; // Fgets returned NULL, stdin probably closed
      }
  }
  return 0;
}