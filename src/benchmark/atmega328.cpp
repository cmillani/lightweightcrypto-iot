#include "Arduino.h"

#include <SPI.h>
#include <inttypes.h>
#include "peer/peer.h"
#include "rfid/MFRC522.h"
#include "Ultrasonic.h"
#include "SoftwareSerial.h"

void setup();
void loop();

//////// Command Bluetooth Serial
SoftwareSerial mySerial(5, 6); // RX, TX

//////// Crypto
PeerState client;
unsigned char msg[MSG_LEN] = "HELLO WORLD";
unsigned char cypher[CYPHER_LEN];

//////// MODE
#define TAG_MODE_PIN 14 // A0
#define LED_MODE_PIN 15 // A1
#define US_MODE_PIN 16 // A2

typedef enum MODE {
  DISTANCE_MODE = 0x01, 
  TAG_MODE = 0x02, 
  LED_MODE = 0x03, 
  NO_MODE = 0x04,
} EMODE;
void setup_modeselector(int usModePin, int tagModePin, int ledModePin);
MODE get_mode(int usModePin, int tagModePin, int ledModePin);

//////// LED
#define LED_PIN 8
void send_led();
void setup_led();

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
  mySerial.begin(9600);
  setup_rfid();
  setup_led();
  setup_modeselector(US_MODE_PIN, TAG_MODE_PIN, LED_MODE_PIN);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop()
{
  MODE mode = get_mode(US_MODE_PIN, TAG_MODE_PIN, LED_MODE_PIN);
  switch (mode) {
    case (DISTANCE_MODE): {
      send_distance();
      break;
    }
    case (TAG_MODE): {
      send_rfid();
      break;
    }
    case (LED_MODE): {
      send_led();
      break;
    }
  }
}

/**
 * Mode Selector
 * 
 */

void setup_modeselector(int usModePin, int tagModePin, int ledModePin) {
  pinMode(5, INPUT_PULLUP);
}

MODE get_mode(int usModePin, int tagModePin, int ledModePin) {
  // This could be simplified accessing the register directly and using a bitmap
  if (digitalRead(usModePin)) {
    return DISTANCE_MODE;
  } else if (digitalRead(tagModePin)) {
    return TAG_MODE;
  } else if (digitalRead(ledModePin)) {
    return LED_MODE;
  } else {
    return NO_MODE;
  }
}

/**
 * LED
 * 
 */

void send_led() {
  digitalWrite(LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_PIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);
  strcpy(msg, "Hello World");
  sendMessage(&client, msg, cypher);
  mySerial.write(cypher, CYPHER_LEN);
}

void setup_led() {
  pinMode(LED_PIN, OUTPUT);
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
  mySerial.write(cypher, CYPHER_LEN);

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
  mySerial.write(cypher, CYPHER_LEN);
  
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