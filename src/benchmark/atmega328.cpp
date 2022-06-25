#include "Arduino.h"

#include <SPI.h>
#include <inttypes.h>
#include "peer/peer.h"
#include "rfid/MFRC522.h"
#include "Ultrasonic.h"

void setup();
void loop();

//////// Crypto
PeerState client;
unsigned char msg[MSG_LEN] = "HELLO WORLD";
unsigned char cypher[CYPHER_LEN];

//////// RFID
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void setup_rfid();
int read_rfid(unsigned char * bytes);
void send_rfid();

//////// Distance
#define TRIGGER_PIN 3
#define ECHO_PIN 2
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

void send_distance();
float get_distance_cm();

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
  send_distance();
  return;
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
 * US Distance Sensor Section
 * 
 */

void send_distance() {
  float distance_cm = get_distance_cm();
  dtostrf(distance_cm, 1, 2, (char *) msg);
  
  Serial.print("Distancia em cm: ");
  Serial.print((char *) msg);
  Serial.println();

  sendMessage(&client, msg, cypher);
  Serial.write(cypher, CYPHER_LEN);
  Serial.println();

  delay(1000);
}

/**
 * @brief Get distance 
 * 
 * @return float, max value is 400
 */
float get_distance_cm() {
  long microsec = ultrasonic.timing();
  float centimeterDistance = ultrasonic.convert(microsec, Ultrasonic::CM);
  if (centimeterDistance > 400.0) {
    centimeterDistance = 400; // Clip Maximum distance to supported value
  }
  return centimeterDistance;
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