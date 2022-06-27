#include <stdio.h>
#include <sys/time.h>
#include "peer/peer.h"

unsigned long diff_time(struct timeval before, struct timeval after) {
    unsigned long micros_before = 1000000 * before.tv_sec + before.tv_usec;
    unsigned long micros_after = 1000000 * after.tv_sec + after.tv_usec;
    return micros_after - micros_before;
}

int main() {
    struct timeval tv_bef, tv_aft; 
    PeerState peer;
    initPeer(&peer);

    unsigned char msg[MSG_LEN] = "Hello Cypher";
    unsigned char recovered_msg[MSG_LEN];
    unsigned char cypher[CYPHER_LEN];

    printf("Enc(micros)\tDec(micros)\n");
    for (int i = 0; i < 200; i++) {
        gettimeofday(&tv_bef,NULL);
        encryptMessage(&peer, msg, cypher);
        gettimeofday(&tv_aft,NULL);
        printf("Enc: %d", diff_time(tv_bef, tv_aft));

        gettimeofday(&tv_bef,NULL);	    
        decryptMessage(&peer, cypher, recovered_msg);
        gettimeofday(&tv_aft,NULL);
        printf("\tDec: %d\n", diff_time(tv_bef, tv_aft));
    }

    return 0;
}
