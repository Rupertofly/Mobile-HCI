/*********************************************************************

   based on Adafruit Bluefruit code.
   MIT license

   This code is designed for use with the Adafruit Bluefruit Feather board

   adapted from code by Scott Mitchell
*********************************************************************/

#include "keyboardSetup.h"

//-----------------------------------------------------------

// Keyboard Name
String s_keyboard_name = "RupertM_Keyboard";

//Send Character
String c_send = "-";
// Pins
const int i_pin_gLED = 2;
const int i_pin_rLED = 3;
const int i_pin_but5_1 = 5;
const int i_pin_but1_1 = 6;
const int i_pin_but1_2 = A2;
const int i_pin_but2_1 = 10;
const int i_pin_but2_2 = A1;
const int i_pin_but3_1 = 11;
const int i_pin_but3_2 = A0;
const int i_pin_but4_1 = 12;
const int i_pin_but4_2 = 13;


//Bytes
byte ya_vals[4];
byte y_loop = 0;
byte y_high = 0;

// Bools
boolean b_send = false;
boolean b_zero = true;
boolean b_receive = false;

// Send Val
int i_send;


//Key Map
const char ca_map[81] = {'0', 'a', 'e', 'i', 0x08, 'n', 'o', 's', ' ', 'u',
                         't', 'h', 'r', '?', 'y', 'd', ',', 'm', '.', 'g',
                         'p', 'c', 'f', 0x21, 'l', 'w', '[', 'x', '~', '~',
                         '~', '~', '~', '~', '~', '~', 'v', '~', '~', 'j',
                         '~', '~', '~', '~', '~', '~', '~', '~', '~', '~',
                         '~', '~', '~', '~', 'q', '~', 'z', '~', '~', '~',
                         '~', '~', '~', '~', '~', '~', '~', '~', '~', '~',
                         '~', '~', 'b', '~', '~', '~', '~', '~', 'k', '~',
                         '~'
                        };


//--------------------------------------------------------
void setup() {
  //Setup keyboard
  setupKeyboard(s_keyboard_name);
  //setup all the pins
  pinMode(i_pin_gLED, OUTPUT);
  pinMode(i_pin_rLED, OUTPUT);
  pinMode(i_pin_but5_1, INPUT_PULLUP);
  pinMode(i_pin_but1_1, INPUT_PULLUP);
  pinMode(i_pin_but2_1, INPUT_PULLUP);
  pinMode(i_pin_but3_1, INPUT_PULLUP);
  pinMode(i_pin_but4_1, INPUT_PULLUP);
  pinMode(i_pin_but1_2, INPUT_PULLUP);
  pinMode(i_pin_but2_2, INPUT_PULLUP);
  pinMode(i_pin_but3_2, INPUT_PULLUP);
  pinMode(i_pin_but4_2, INPUT_PULLUP);

}
//--------------------------------------------------------
void loop() {
  long l_time = millis(); //start timing loop
  byte y_state = ready_state(); //get current button state
  delay(50);
  byte y_output = convert_button(y_state); //convert to 3rd order state
  if (y_output == 0 && b_zero == false){ // if current state is zero, and zero is yet to be received, mark zero received
    b_zero = true;
    y_loop = 0;
  }
  if (y_output != 0 && b_zero) b_receive = true; //if receiving a character other then zero,
  if (y_loop <5&&b_receive){ //if looped less then 5 times and still receiving a characeter other then 0
    if (key_check(y_output)) { //if looped 5 times
      b_send = true; //tell code to send
      i_send = y_high; // tell code what to send
      y_high = 0; //reset values
      b_receive = 0;
    }
  }
  if (b_send) {
    //Serial.print(String(i_send) + " ");
    b_send = false; //reset send values
    b_zero = false;
    if(ca_map[i_send] != '~'){ //if character is not a ~ (placeholder invalid)
      if (ca_map[i_send] == '['){
        c_send = "\\r";
      } else if (ca_map[i_send] == '?'){
        c_send = "\\?";
      } else {
        c_send = String(ca_map[i_send]); //set output character to corresponding character
      }
      digitalWrite(i_pin_gLED, 1); //flash green to signify send
      ble.print("AT+BleKeyboard="); //write character to bluetooth
      ble.println(c_send);

      if ( ble.waitForOK() ) //if send successfull
      {
        digitalWrite(i_pin_gLED, 0); //stop green led
      }  else
      {
        digitalWrite(i_pin_gLED, 0); //flash red led and turn of green led
        digitalWrite(i_pin_rLED, 1);
        delay(50);
        digitalWrite(i_pin_rLED, 0);
      }
    }
  }
}
byte ready_state() { //get binary indication of buttons pressed
  // start with them all off
  byte y_state = B00000000;

  // turn on the bits if the button is being pressed
  if (digitalRead(i_pin_but1_1) == false) bitSet(y_state, 0);
  if (digitalRead(i_pin_but1_2) == false) bitSet(y_state, 1);
  if (digitalRead(i_pin_but2_1) == false) bitSet(y_state, 2);
  if (digitalRead(i_pin_but2_2) == false) bitSet(y_state, 3);
  if (digitalRead(i_pin_but3_1) == false) bitSet(y_state, 4);
  if (digitalRead(i_pin_but3_2) == false) bitSet(y_state, 5);
  if (digitalRead(i_pin_but4_1) == false) bitSet(y_state, 6);
  if (digitalRead(i_pin_but4_2) == false) bitSet(y_state, 7);


  return (y_state);
}

int convert_button(byte _y_b) { //convert binary into a number index number
  for (int i = 0; i < 4; i++) {
    int i_pos = i * 2;
    byte y_exb;
    bitWrite(y_exb, 0, bitRead(_y_b, i_pos));
    bitWrite(y_exb, 1, bitRead(_y_b, i_pos + 1));
    if (y_exb == 3) y_exb = 2;
    ya_vals[i] = y_exb;
  }
  int but_int_val = (ya_vals[0] + (ya_vals[1] * 3) + (ya_vals[2] * 9) + (ya_vals[3] * 27));
  return (but_int_val);
}

boolean key_check(byte _y_key){ //check whether current value is higher then previous values, and increment loop count
  if (_y_key!=0){
    if (_y_key > y_high){
      y_high = _y_key;
    }
  }
  if(y_loop >= 4){
    return true;
    //y_loop=0;
  } else {
    y_loop++;
    return false;
  }
}
