#include "Arduino.h"

#include <SPI.h>
#include <inttypes.h>
#include "peer/peer.h"
#include "rfid/MFRC522.h"

void setup();
void loop();

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

const unsigned char key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

PeerState client;
unsigned char msg[MSG_LEN] = "HELLO WORLD";
unsigned char cypher[CYPHER_LEN];

void setup_rfid();
int read_rfid(unsigned char * bytes);
void send_rfid();

void setup()
{
  initPeer(&client);
  Serial.begin(9600);
  setup_rfid();
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop()
{
  send_rfid();
  return;
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);
  sendMessage(&client, msg, cypher);
  Serial.write(cypher, CYPHER_LEN);
}

/**
 * RFID Section
 * 
 */

void send_rfid() {
  memset(msg, 0, MSG_LEN * sizeof(unsigned char));
  int size = read_rfid(msg);
  if (size == 0) {
    return;
  }
  for (byte i = 0; i < size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  sendMessage(&client, msg, cypher);
  Serial.write(cypher, CYPHER_LEN);
  Serial.println();
  delay(1000);
}

void setup_rfid()
{
  SPI.begin();        // Inicia  SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522
}

int read_rfid(unsigned char * bytes)
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }

  memcpy(bytes, mfrc522.uid.uidByte, mfrc522.uid.size * sizeof(byte));
  return mfrc522.uid.size;
}