// button debouncing: https://docs.arduino.cc/built-in-examples/digital/Debounce/
// with buzzer as feedback system

#include "IRremote.h"
#include <Adafruit_NeoPixel.h>

#define IR_PIN 2 // D2
#define BUTTON 3 // D3
#define BUZZER 4 // D4

#define NUMPIXELS 6

const uint8_t MAG_SIZE = 6;
uint8_t remainingBullets = MAG_SIZE;

int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 30;    // the debounce time; increase if the output flickers

int RED_ENCODING_VALUE = 0xFF708F;
uint8_t sCommand = 0x06; // IR command for our team
uint8_t sRepeats = 4; // Repeat IR Signal for each transmission to improve the range.

void test_IR();
void buzzer_feed_back();
void read_button();

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);
  IrSender.begin(IR_PIN);
}

void loop() {
  read_button();
}

void testIR() {
  IrSender.sendNEC(0x00, sCommand, sRepeats);
  delay(10);
  Serial.println("IR Signal sent.");
  delay(2000);
}

void buzzer_feed_back() {
  tone(BUZZER, 1000/remainingBullets); // Send 1KHz sound signal...
  delay(500);
  noTone(BUZZER);
}

void read_button() {
  int reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
      lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH && remainingBullets > 0) {
        Serial.println("IR Signal Sent (button)");
        IrSender.sendNEC(0x00, sCommand, sRepeats); // use NEC protocol
        delay(20); // delay for sending IR signal;
        buzzer_feed_back();
        remainingBullets -= 1;
      }
    }
  }
  lastButtonState = reading;
}

