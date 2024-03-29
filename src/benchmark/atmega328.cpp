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
unsigned char msg[MSG_LEN] = {0};
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
MODE get_mode();

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
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Booted!");
}

void loop()
{
  MODE mode = get_mode();
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

MODE get_mode() {
  static MODE lastMode = NO_MODE;
  unsigned char mode = lastMode;
  if (Serial.available()) {
    Serial.readBytes(&mode, 1);
    mode -= '0'; // Converts ascii to integer
    switch (mode) {
      case (TAG_MODE): {
        mode = TAG_MODE;
        break;
      }
      case (DISTANCE_MODE): {
        mode = DISTANCE_MODE;
        break;
      }
      case (LED_MODE): {
        mode = LED_MODE;
        break;
      }
      default: {
        mode = NO_MODE;
        break;
      }
    }
  }
  if (mode != lastMode) {
    lastMode = mode;
    Serial.print("Changing mode to: ");
    Serial.println(mode);
  }
  
  return mode;
}

/**
 * LED
 * 
 */

void send_led() {
  if (mySerial.available()) {
    Serial.println("Reading Data");
    mySerial.readBytes(cypher, CYPHER_LEN);
    int res = decryptMessage(&client, cypher, msg);
    if (res == 0) {
      Serial.write(msg, MSG_LEN);
      Serial.println();
      if (msg[0] == '0') digitalWrite(LED_PIN, LOW);
      else if (msg[0] == '1') digitalWrite(LED_PIN, HIGH);
    }
  }
  delay(100);
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
  memset(msg, 0, MSG_LEN);
  dtostrf(distance_cm, 1, 2, (char *) msg);
  
  Serial.print("Distancia em cm: ");
  Serial.print((char *) msg);
  Serial.println();

  encryptMessage(&client, msg, cypher);
  mySerial.write(cypher, CYPHER_LEN);

  delay(100);
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
  unsigned char buffer[10] = {0};
  int size = read_rfid(buffer);
  if (size == 0) {
    return;
  }

  for (byte i = 0; i < size; i++) {
    sprintf(msg + (2*i), "%02x", buffer[i]);
  }
  Serial.println((char *)msg);

  encryptMessage(&client, msg, cypher);
  mySerial.write(cypher, CYPHER_LEN);
  
  delay(500);
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