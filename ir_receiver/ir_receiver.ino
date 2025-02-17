#include <Arduino.h>
#include <IRremote.hpp>  // Include IR remote library
#include <Adafruit_NeoPixel.h>
#include "CRC8.h"

#define LED_PIN 3 // D3
#define NUMPIXELS 12 // Number of pixels for LED RING
#define RECV_PIN 2 // D2


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t MAX_SHIELD = 30;
const uint8_t MAX_HEALTH = 100;
const uint8_t RESPONSE_TIMEOUT = 1000;
const uint8_t MAX_RESEND_COUNT = 3;

uint8_t shield = 0;
uint8_t health = MAX_HEALTH;
unsigned long lastVestShotTime = 0;
uint8_t sCommand = 0x06;

void applyDamage(uint8_t damage);
void updateLED();
void handleIRSignal();
void testLED();

void circleLED();

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  pixels.begin();
  pixels.show();
  updateLED();
}

void loop() {
  handleIRSignal();
  // // Handle retransmission if no ACK received
  // if (waitingForVestACK) {
  //   if ((millis() - lastVestShotTime > RESPONSE_TIMEOUT) && (packetResendCount < MAX_RESEND_COUNT)) {
  //     sendPacket(VESTSHOT_PACKET);
  //     packetResendCount++;
  //     lastVestShotTime = millis();
  //   }
  // }
}

void testLED() {
  for (int i = 100; i > 0; i -= 8) {
    health = i;
    updateLED();
  }
  delay(5000);
}

void handleIRSignal() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.command == sCommand && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {  // Assuming 0xE is the damage signal
      // applyDamage(5);
      // sendPacket(VESTSHOT_PACKET);
      // waitingForVestACK = true;
      health -= 10;
      updateLED();
      // Serial.println("IR Received.");
      lastVestShotTime = millis();
    }
    IrReceiver.resume();
  }
}

void applyDamage(uint8_t damage) {
  if (shield >= damage) {
    shield -= damage;
  } else {
    uint8_t remainingDamage = damage - shield;
    shield = 0;
    health = (health > remainingDamage) ? health - remainingDamage : 0;
  }
}

void updateLED() {
  int full_leds = min(health/8, 12);  

  for(int i = 0; i < NUMPIXELS; i++) {
    if (i < full_leds) {
      pixels.setPixelColor(i,pixels.Color(0,0,20));
    } else {
      pixels.setPixelColor(i,pixels.Color(20,0,0));
    }
    pixels.show();
    delay(100);
  }
}

void circleLED() {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i,pixels.Color(20,0,0));
    pixels.show();
    delay(100);
  }

  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i,pixels.Color(0,0,20));
    pixels.show();
    delay(100);
  }
} 