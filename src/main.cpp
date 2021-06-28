/*
*  Tally Light Controller and Transmitter
*  Reads switches from D-Sub 15 port on controller using A0-3 for green pre-stage and 12-9 for red active
*  transmits port color when it changes with periodic refresh (in case a signal is missed)
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <elapsedMillis.h>

#define setPin 6
#define RXPin 2
#define TXPin 3

int iPin;
char cPin;
int kPIN_MAX = 4;
int kPIN_GREEN_SW[] = {A0, A1, A2, A3};
int kPIN_RED_SW[] = {12, 11, 10, 9};
char kFEED[] = {'1','2','3','4'};
char cCurrColor;
char cPrevColor[] = {'O', 'O', 'O', 'O'};
char kGREEN = 'G';
char kRED = 'R';
char kOFF = 'O';
char kRESEND = ' ';
int iSwOn;


SoftwareSerial HC12(TXPin, RXPin);         // HC-12 TX Pin, HC-12 RX Pin
byte incomingByte;
String writeBuffer = "";

elapsedMillis iResetTime;
int kI_RESET_TIME = 2000;

void setup() {
  HC12.begin(9600);                     // Open serial port to HC12
  pinMode(setPin, OUTPUT);
  digitalWrite(setPin, LOW);
  delay(250);
  HC12.write("AT+DEFAULT");
  delay(250);
  digitalWrite(setPin, HIGH);           // HC-12 normal, transparent mode
  for (iPin = 0; iPin < kPIN_MAX; iPin++) {
    pinMode(kPIN_GREEN_SW[iPin], INPUT_PULLUP);
    pinMode(kPIN_RED_SW[iPin], INPUT_PULLUP);
  }
}

void loop() {
  delay(100);
 
  for (iPin = 0; iPin < kPIN_MAX; iPin++) {
    writeBuffer = "";
    writeBuffer += kFEED[iPin];
    iSwOn = !digitalRead(kPIN_RED_SW[iPin]);
    if (iSwOn == HIGH) {
      cCurrColor = kRED;
    } else {
      iSwOn = !digitalRead(kPIN_GREEN_SW[iPin]);
      if (iSwOn == HIGH) {
        cCurrColor = kGREEN;
      } else {
        cCurrColor = kOFF;
      }
    }
    if (cCurrColor != cPrevColor[iPin]) {
       writeBuffer+= cCurrColor;
      cPrevColor[iPin] = cCurrColor;
      HC12.print(writeBuffer);
      delay(100);
    }
  }

  if (iResetTime >= kI_RESET_TIME) {
    iResetTime = 0;
    for (iPin = 0; iPin < kPIN_MAX; iPin++) {
      cPrevColor[iPin] = kRESEND;
    }
  }

}