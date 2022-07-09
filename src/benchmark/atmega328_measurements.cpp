#include "Arduino.h"

#include <SPI.h>
#include <inttypes.h>
#include "peer/peer.h"
#include "rfid/MFRC522.h"
#include "Ultrasonic.h"
#include "SoftwareSerial.h"

void setup();
void loop();

//////// Crypto
PeerState client;
unsigned char msg[MSG_LEN] = "Hello World!";
unsigned char cypher[CYPHER_LEN];

int iteration = 0;

void setup()
{
  initPeer(&client);
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Booted!");
}

void loop()
{
  if (iteration >= 100) {
    return;
  }
  iteration++;
  
  unsigned long before, after;
  int ret;

  before = micros();
  ret = encryptMessage(&client, msg, cypher);
  after = micros();
  Serial.print("Enc: ");
  Serial.print(after - before);

  if (ret != 0) {
    Serial.println("Error Encrypting!");
  }

  before = micros();
  ret = decryptMessage(&client, cypher, msg);
  after = micros();
  Serial.print("\tDec: ");
  Serial.println(after - before);

  if (ret != 0) {
    Serial.println("Error Decrypting!");
  }
  delay(100);
}