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
String c_send = "-";

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
byte but_vals[4];
byte out_val[3];
byte read_pos = 0;
boolean send_key = false;
int send_val;
boolean z_receive = true;

const char keyMap[64] = {'0', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                         'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
                         'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ';', 'P', 'Q',
                         'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ',
                         '.', ',', '?', '!', '/', '(', ')'
                        };



void setup() {
  Serial.begin(9600);
  setupKeyboard(KEYBOARD_NAME);
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
void loop() {
  long loop_start = millis();
  byte buttonState = readButtonState();
  delay(75);
  int output_int = convert_button(buttonState);
  if (output_int == 0 && z_receive == false) z_receive = true;
  if (read_pos > 2) read_pos = 0;
  if (z_receive) {
    switch (read_pos) {
      case 0:
        out_val[0] = output_int;
        read_pos++;
        break;
      case 1:
        out_val[1] = output_int;
        if (out_val[0] == out_val[1] && out_val[0] != 0) {
          read_pos++;
        } else {
          read_pos = 0;
        }
        break;
      case 2:
        out_val[2] = output_int;
        if (out_val[1] == out_val[2] && out_val != 0) {
          send_key = true;
          send_val = out_val[2];
          read_pos = 0;
        } else {
          read_pos = 0;
        }
        break;
      default:
        read_pos = 0;
        break;
    }
  }
  if (send_key && z_receive) {
    Serial.print(String(send_val) + " ");
    send_key = false;
    z_receive = false;
    c_send = String(keyMap[send_val]);
    digitalWrite(pin_gLED, 1);
    ble.print("AT+BleKeyboard=");
    ble.println(c_send);

    if ( ble.waitForOK() )
    {
      digitalWrite(pin_gLED, 0);
    }  else
    {
      digitalWrite(pin_gLED, 0);
      digitalWrite(pin_rLED, 1);
      delay(50);
      digitalWrite(pin_rLED, 0);
    }
  } else {
    Serial.print("0 ");
  }

  boolean b5 = digitalRead(pin_but5_1);
  Serial.print(output_int);

  Serial.print(" ");
  Serial.print(!b5);
  Serial.print(" ");
  Serial.print(String(read_pos) + " ");


  Serial.println(millis() - loop_start);


  if (b5 == 0) {
    digitalWrite(pin_rLED, 1);
  }
  else {
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

int convert_button(byte b) {
  byte out_v[4];
  for (int i = 0; i < 4; i++) {
    int pos = i * 2;
    byte exb;
    bitWrite(exb, 0, bitRead(b, pos));
    bitWrite(exb, 1, bitRead(b, pos + 1));
    if (exb == 3) exb = 2;
    but_vals[i] = exb;
  }
  int but_int_val = (but_vals[0] + (but_vals[1] * 3) + (but_vals[2] * 9) + (but_vals[3] * 27));
  return (but_int_val);
}
