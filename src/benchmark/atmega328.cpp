#include "Arduino.h"

#include <inttypes.h>
#include "peer/peer.h"

void setup();
void loop();

const unsigned char key[32] = "000102030405060708090A0B0C0D0E0F";

PeerState client;
unsigned char msg[MSG_LEN] = "HELLO WORLD";
unsigned char cypher[CYPHER_LEN];

void setup() {
    initPeer(&client);
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}
                                                 
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);
    sendMessage(&client, msg, cypher);
    Serial.write(cypher, CYPHER_LEN);
}