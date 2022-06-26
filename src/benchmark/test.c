#include <stdio.h>
#include "peer/peer.h"

int main() {
    PeerState peer;
    initPeer(&peer);

    unsigned char msg[MSG_LEN] = "Hello Cypher";
    unsigned char recovered_msg[MSG_LEN];
    unsigned char cypher[CYPHER_LEN];

    printf("Original Msg: '%s'\n", msg);
    encryptMessage(&peer, msg, cypher);
    printf("Cypher (hex)::\n");
    for (int i = 0; i < CYPHER_LEN; i++) {
        printf("%02x", cypher[i]);
    }
    printf("\n");
    decryptMessage(&peer, cypher, recovered_msg);
    printf("Recovered Msg: '%s'\n", recovered_msg);

    return 0;
}