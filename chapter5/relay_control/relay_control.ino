/*************************************************** 
  This is an example for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA 
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963
  ----> http://www.adafruit.com/products/2468
  ----> http://www.adafruit.com/products/2542

  These cellular modules use TTL Serial to communicate, 2 pins are 
  required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Include library
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

// Pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#define RELAY_PIN 7
#define LED_PIN 8

// Lock control
bool lock_state = false;
int init_counter = 0;
int lock_counter = 0;
int lock_delay = 5000;

// SMS variables
int8_t smsnum = 0;
int8_t smsnum_old = 0;

// Password
String password_on = "open";
String password_off = "close";

// Buffer for replies
char replybuffer[255];

// Software serial
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Fona
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Readline function
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {

  // Set output pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Start Serial
  while (!Serial);
  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  // Init FONA
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }

}

void loop() {

  // Get total number of SMS
  smsnum = fona.getNumSMS();

  // Read last SMS if new message
  if (smsnum > smsnum_old) {
    
    flushSerial();
    Serial.print(F("Read #"));
    Serial.print(F("\n\rReading SMS #")); Serial.println(smsnum);

    // Retrieve SMS sender address/phone number.
    if (! fona.getSMSSender(smsnum, replybuffer, 250)) {
      Serial.println("Failed!");
    }
    Serial.print(F("FROM: ")); Serial.println(replybuffer);

    // Retrieve SMS value.
    uint16_t smslen;
    if (! fona.readSMS(smsnum, replybuffer, 250, &smslen)) { // pass in buffer and max len!
      Serial.println("Failed!");
    }
    Serial.print(F("***** SMS #")); Serial.print(smsnum); 
    Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
    Serial.println(replybuffer);
    Serial.println(F("*****"));

    // Control the relay depending on the received message
    String message = String(replybuffer);
  
    // Relay & LED ON
    if (message.indexOf(password_on) > -1) {
      Serial.println("Lock OPEN");
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      lock_state = true;
      init_counter = millis();
    }
  
    // Relay & LED OFF
    if (message.indexOf(password_off) > -1) {
      Serial.println("Lock CLOSE");
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      lock_state = false;
    }
  
  }

  // Update timer
  lock_counter = millis();

  // If timer above delay, close the lock again
  if (lock_state == true && (lock_counter - init_counter) > lock_delay) {
    Serial.println("Lock CLOSE");
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    lock_state = false;
  }

  // Update SMS count
  smsnum_old = smsnum;
  
  // Flush input
  flushSerial();
  while (fona.available()) {
    Serial.write(fona.read());
  }

  // Wait 1 second
  delay(1000);

}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}
  
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;
  
  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;
        
        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }
    
    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
