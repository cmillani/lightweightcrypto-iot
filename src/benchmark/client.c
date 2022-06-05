#include <stdio.h>
#include <string.h>
#include "peer/peer.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Must receive pipe as parameter, as 'client >(server)'");
    return 1;
  }
  FILE * serverPipe = fopen(argv[1], "w");

  PeerState client;
  initPeer(&client);

  unsigned char msg[MSG_LEN];
  unsigned char cypher[CYPHER_LEN];

  printf("Write Input Data:\n");
  for(;;) {
      
      char * ret = fgets((char *) msg, sizeof(msg), stdin);
      if (ret != NULL) {
        sendMessage(&client, msg, cypher);
        int count = fwrite(cypher, sizeof(unsigned char), CYPHER_LEN, serverPipe);
        if (count != CYPHER_LEN) {
          printf("ERROR: Something prevented writing all data, ferror:%d", ferror(serverPipe));
          return 1;
        }
        fflush(serverPipe); // Sends this block immediatelly
      } else {
        break; // Fgets returned NULL, stdin probably closed
      }
  }

  fclose(serverPipe);
  return 0;
}