/*********************************************************************

  Human Interface Design (HID)
  Basic Chorded Keyboard
  with chord delay and auto repeat

  by Scott Mitchell
  RMIT University

  based on Adafruit Bluefruit code.
  MIT license

  This code is designed for use with the Adafruit Bluefruit Feather board

*********************************************************************/

#include "keyboardSetup.h"
String KEYBOARD_NAME = "RupertM_Keyboard";

const int pin_gLED = 2;
const int pin_rLED = 3;
const int pin_but5_1 = 5;
const int pin_but1_1 = 6;
const int pin_but1_2 = A2;
const int pin_but2_1 = 10;
const int pin_but2_2 = A1;
const int pin_but3_1 = 11;
const int pin_but3_2 = A0;
const int pin_but4_1 = 12;
const int pin_but4_2 = 13;


void setup(){
  Serial.begin(9600);
  pinMode(pin_gLED, OUTPUT);
  pinMode(pin_rLED, OUTPUT);
  pinMode(pin_but5_1, INPUT_PULLUP);
  pinMode(pin_but1_1, INPUT_PULLUP);
  pinMode(pin_but2_1, INPUT_PULLUP);
  pinMode(pin_but3_1, INPUT_PULLUP);
  pinMode(pin_but4_1, INPUT_PULLUP);
  pinMode(pin_but1_2, INPUT_PULLUP);
  pinMode(pin_but2_2, INPUT_PULLUP);
  pinMode(pin_but3_2, INPUT_PULLUP);
  pinMode(pin_but4_2, INPUT_PULLUP);

}
void loop(){
  byte buttonState = readButtonState();
  boolean b5 = digitalRead(pin_but5_1);
  Serial.print(buttonState, BIN);
  Serial.print(" ");
  Serial.println(!b5);
  if(b5==0){
    digitalWrite(pin_gLED, 1);
    digitalWrite(pin_rLED, 1);
  }
  else{
    digitalWrite(pin_gLED, 0);
    digitalWrite(pin_rLED, 0);
  }
}
byte readButtonState() {
  // start with them all off
  byte bState = B00000000;

  // turn on the bits if the button is being pressed
  if (digitalRead(pin_but1_1) == false) bitSet(bState, 0);
  if (digitalRead(pin_but1_2) == false) bitSet(bState, 1);
  if (digitalRead(pin_but2_1) == false) bitSet(bState, 2);
  if (digitalRead(pin_but2_2) == false) bitSet(bState, 3);
  if (digitalRead(pin_but3_1) == false) bitSet(bState, 4);
  if (digitalRead(pin_but3_2) == false) bitSet(bState, 5);
  if (digitalRead(pin_but4_1) == false) bitSet(bState, 6);
  if (digitalRead(pin_but4_2) == false) bitSet(bState, 7);


  return (bState);
}
