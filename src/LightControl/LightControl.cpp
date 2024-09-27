
#include "Arduino.h"
#include "LightControl.h"
#define Effect
#ifdef Effect 
#ifdef LightControl
1 oe
2 
3 rd1
4 rd2
5 rd3
6 rd4
7 gd1
8 gd2
3 gd3
3 gd4
9 clk
33 lat
35 D
36 A
37 B
38 C
39 RTC_CLK
40 RTC_DAT
41 RTC_ENA
46 BNT

#define dataPin = 12;

#define latchPin = 14;

#define clockPin = 27;
#endif//BOARD_LEDController
int dataPin = 3;
int latchPin = 33;
int clockPin = 9;
#define PIN_ENA 41
#define PIN_CLK 39
#define PIN_DAT 40
#define rxPin 44
#define txPin 43
const int SerialBaudrate = 115200;  // or 19200 usually

#include <Ds1302.h>

// DS1302 RTC instance
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

const static char* WeekDays[] =
{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

HardwareSerial MySerial0(0);

#define t1 10

#define t2 100

#define t3  50

//How many of the shift registers - change this

#define number_of_74hc595s 8



//do not touch

#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

void RTC_Get(){
  // get the current time
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    static uint8_t last_second = 0;
    if (last_second != now.second)
    {
        last_second = now.second;

        MySerial0.print("20");
        MySerial0.print(now.year);    // 00-99
        MySerial0.print('-');
        if (now.month < 10) MySerial0.print('0');
        MySerial0.print(now.month);   // 01-12
        MySerial0.print('-');
        if (now.day < 10) MySerial0.print('0');
        MySerial0.print(now.day);     // 01-31
        MySerial0.print(' ');
        MySerial0.print(WeekDays[now.dow - 1]); // 1-7
        MySerial0.print(' ');
        if (now.hour < 10) MySerial0.print('0');
        MySerial0.print(now.hour);    // 00-23
        MySerial0.print(':');
        if (now.minute < 10) MySerial0.print('0');
        MySerial0.print(now.minute);  // 00-59
        MySerial0.print(':');
        if (now.second < 10) MySerial0.print('0');
        MySerial0.print(now.second);  // 00-59
        MySerial0.println();
    }
}
void Light_setup() {

  pinMode(dataPin, OUTPUT);

  pinMode(latchPin, OUTPUT);

  pinMode(clockPin, OUTPUT);
    MySerial0.begin(SerialBaudrate, SERIAL_8N1, rxPin, txPin);
    MySerial0.println("MySerial0");

        // initialize the RTC
    rtc.init();

    // test if clock is halted and set a date-time (see example 2) to start it
    if (rtc.isHalted())
    {
        MySerial0.println("RTC is halted. Setting time...");

        Ds1302::DateTime dt = {
            .year = 24,
            .month = Ds1302::MONTH_SEP,
            .day = 27,
            .hour = 1,
            .minute = 20,
            .second = 30,
            .dow = Ds1302::DOW_FRI
        };

        rtc.setDateTime(&dt);
    }
//   pinMode(outputEnable, OUTPUT);

//   Serial.begin(9600);

  clearLed(0);

  turnOutputsOn();

}

void Light_loop() {
  RTC_Get();
  effect_8(1);

  clearLed(0);

  effect_1(1);

  clearLed(0);

  effect_2(2);

  clearLed(0);

  effect_3(2);

  clearLed(0);

  effect_4(4);

  clearLed(0);

  effect_5(2);

  clearLed(0);

  effect_6(1);

  clearLed(0);

  effect_7(2);

  clearLed(0);

  clearLed(0);

  effect_9(1);

  clearLed(0);

  effect_10(1);

  clearLed(0);

  effect_11(2);

  clearLed(0);

  effect_12(1);

  clearLed(0);

  effect_13(1);

  clearLed(0);

  effect_14(1);

  clearLed(0);

  effect_15(4);

  clearLed(0);

  effect_16(1);

  clearLed(0);

  effect_17(1);

  clearLed(0);

  effect_18(4);

  clearLed(0);

  effect_4(4);

}

//////////////////////////////////////////////////////////// Effect 1
byte boiso = 3;

void effect_1(int effectcount) {
Serial.println("Effect 1");
  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < 4; i++) {

      registersWrite(i, HIGH, 5);

      delay(200);

    }

    clearLed(10);

    for (int i = 4; i < 7; i++) {

      registersWrite(i, HIGH, 5);

      delay(200);

    }

    clearLed(10);

    for (int i = 7; i < 10; i++) {

      registersWrite(i, HIGH, 5);

      delay(200);

    }

    clearLed(10);

    for (int i = 10; i < 13; i++) {

      registersWrite(i, HIGH, 5);

      delay(200);

    }

    clearLed(10);

    for (int i = 13; i < 16; i++) {

      registersWrite(i, HIGH, 5);

      delay(200);

    }

    clearLed(10);

  }

}

/////////////////////////////////////////////////////////////Effect 2

void effect_2(int effectcount) {
Serial.println("Effect 2");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

      delay(t1);

      registersWrite(i + 1, HIGH, 5);

      delay(t1);

      registersWrite(i + 2, HIGH, 5);

      delay(t1);

      registersWrite(i + 3, HIGH, 5);

      delay(t1);

      registersWrite(i, LOW, 5);

      delay(t1);

      registersWrite(i + 1, LOW, 5);

      delay(t1);

    }

    clearLed(0);

    for (int i = (number_of_74hc595s * 8) - 1; i >= 0; i--) {

      registersWrite(i, HIGH, 5);

      delay(t1);

      registersWrite(i - 1, HIGH, 5);

      delay(t1);

      registersWrite(i - 2, HIGH, 5);

      delay(t1);

      registersWrite(i - 3, HIGH, 5);

      delay(t1);

      registersWrite(i, LOW, 5);

      delay(t1);

      registersWrite(i - 1, LOW, 5);

      delay(t1);

    }

  }

}

////////////////////////////////////////////////////////Effect 3

void effect_3(int effectcount) {
Serial.println("Effect 3");

  for (int j = 1; j <= effectcount; j++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

      delay(50);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, LOW, 5);

      delay(50);

    }

  }

}

//////////////////////////////////////////////////////////Effect 4

void effect_4(int effectcount) {
Serial.println("Effect 4");

  for (int j = 1; j <= effectcount; j++) {

    byte led = 0b10101010;

    digitalWrite(latchPin, LOW);
    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led);}

    digitalWrite(latchPin, HIGH);

    delay(100);



    byte led1 = 0b01010101;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}

    digitalWrite(latchPin, HIGH);

    delay(100);

  }

}

/////////////////////////////////////////////////////////Effect 5

void effect_5(int effectcount) {
Serial.println("Effect 5");

  for (int j = 1; j <= effectcount; j++) {

    byte led = 0b11111111;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led);}

    digitalWrite(latchPin, HIGH);

    delay(100);
    byte led1 = 0b00000000;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}

    digitalWrite(latchPin, HIGH);

    delay(100);

  }

}

/////////////////////////////////////////////////////////Effect 6

void effect_6(int effectcount) {
Serial.println("Effect 6");

  for (int j = 1; j <= effectcount; j++) {

    int count = number_of_74hc595s * 8;

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      clearLed(0);

      registersWrite(i, HIGH, 0);  // chaser 1

      registersWrite(count, HIGH, 0); // chaser 2

      count--;

    }

  }

}

/////////////////////////////////////////////////////////////Effect 7

void effect_7(int effectcount) {
Serial.println("Effect 7");

  for (int i = 1; i <= effectcount; i++) {

    int count2 = number_of_74hc595s * 8;

    for (int j = 0; j < number_of_74hc595s * 8; j++) {

      clearLed(0);

      registersWrite(j, HIGH, 0);  // chaser 1

      registersWrite(j - 1, HIGH, 0);

      registersWrite(count2, HIGH, 0); // chaser 2

      registersWrite(count2 - 1, HIGH, 0); // chaser 2

      count2--;

    }

  }

}

///////////////////////////////////////////////////////////////Effect 8

byte dataLed[24] = {
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B11111111,
  0B11111110,
  0B11111100,
  0B11111000,
  0B11110000,
  0B11100000,
  0B11000000,
  0B10000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000
};
void effect_8(int effectcount) {
Serial.println("Effect 8");

    byte led = 0B11111111;
    byte led1 = 0B00000000;
    
    digitalWrite(latchPin, LOW);
    for(byte a = 0 ; a < number_of_74hc595s; a++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}
    digitalWrite(latchPin, HIGH);
    delay(2000);
  for (int i = 1; i <= effectcount; i++) {
        for(int b = 0 ; b < 10; b+=1){
            for(int i = 0 ; i < 300; i++){   
                digitalWrite(latchPin, LOW);
                for(byte a = 0 ; a < number_of_74hc595s; a++){
                    shiftOut(dataPin, clockPin, MSBFIRST, led);}
                digitalWrite(latchPin, HIGH);
                delayMicroseconds(b*10);
                digitalWrite(latchPin, LOW);
                for(byte a = 0 ; a < number_of_74hc595s; a++){
                    shiftOut(dataPin, clockPin, MSBFIRST, led1);}
                digitalWrite(latchPin, HIGH);
                delayMicroseconds((10-b)*10);
            }
        }
        
    digitalWrite(latchPin, LOW);
    for(byte a = 0 ; a < number_of_74hc595s; a++){
        shiftOut(dataPin, clockPin, MSBFIRST, led);}
    digitalWrite(latchPin, HIGH);
    delay(2000);
        for(int b = 10 ; b > 0; b-=1){
            for(int i = 0 ; i < 300; i++){   
                digitalWrite(latchPin, LOW);
                for(byte a = 0 ; a < number_of_74hc595s; a++){
                    shiftOut(dataPin, clockPin, MSBFIRST, led);}
                digitalWrite(latchPin, HIGH);
                delayMicroseconds(b*10);
                digitalWrite(latchPin, LOW);
                for(byte a = 0 ; a < number_of_74hc595s; a++){
                    shiftOut(dataPin, clockPin, MSBFIRST, led1);}
                digitalWrite(latchPin, HIGH);
                delayMicroseconds((10-b)*10);
            }
        }
    digitalWrite(latchPin, LOW);
    for(byte a = 0 ; a < number_of_74hc595s; a++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}
    digitalWrite(latchPin, HIGH);

  }

}



//////////////////////////////////////////////////////////////Effect 9

void effect_9(int effectcount) {
Serial.println("Effect 9");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      for (int k = i; k < number_of_74hc595s * 8; k++) {

        registersWrite(k, HIGH, 5);

        delay(10);

        registersWrite(k, LOW, 5);

        delay(10);

      }

      registersWrite(i, HIGH, 5);

      delay(20);

    }

  }

}

//////////////////////////////////////////////////////////////////Effect 10

void effect_10(int effectcount) {
Serial.println("Effect 10");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = (number_of_74hc595s * 8) - 1; i >= 0; i--) {

      for (int k = 0; k < i; k++) {

        registersWrite(k, HIGH, 5);

        delay(10);

        registersWrite(k, LOW, 5);

        delay(10);

      }

      registersWrite(i, HIGH, 5);

      delay(20);

    }

  }

}

////////////////////////////////////////////////////////////Effect 11

void effect_11(int effectcount) {
Serial.println("Effect 11");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

      delay(30);

      registersWrite(i + 2, LOW, 5);

      delay(30);

      registersWrite(i - 5, LOW, 5);

    } clearLed(0);

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, HIGH, 5);

      delay(30);

      registersWrite(i - 2, LOW, 5);

      delay(30);

      registersWrite(i + 5, LOW, 5);

    }

  }

}



///////////////////////////////////////////////////////////////Effect 12

void effect_12(int effectcount) {
Serial.println("Effect 12");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < (number_of_74hc595s * 8)/2; i++) {

      for (int k = i; k < (number_of_74hc595s * 8)/2; k++)

      {

        registersWrite(k, HIGH, 5);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 5);

        delay(20);

        registersWrite(k, LOW, 5);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 5);

        delay(20);

      }

      registersWrite(i, HIGH, 5);

      registersWrite((number_of_74hc595s * 8)-1 - i, HIGH, 5);

    }

  }

}

/////////////////////////////////////////////////////////////////Effect 13

void effect_13(int effectcount) {
Serial.println("Effect 13");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = ((number_of_74hc595s * 8)/2)-1; i >= 0; i--) {

      for (int k = 0; k <= i; k++)

      {

        registersWrite(k, HIGH, 5);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 5);

        delay(20);

        registersWrite(k, LOW, 5);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 5);

        delay(20);

      }

      registersWrite(i, HIGH, 5);

      registersWrite(((number_of_74hc595s * 8)-1) - i, HIGH, 5);

    }

  }

}

////////////////////////////////////////////////////////////Effect 14

void effect_14(int effectcount) {
Serial.println("Effect 14");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

      delay(t3);

    }

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, LOW, 5);

      delay(t3);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, HIGH, 5);

      delay(t3);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, LOW, 5);

      delay(t3);

    }

  }

}

/////////////////////////////////////////////////////////////Effect 15

void effect_15(int effectcount) {
Serial.println("Effect 15");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

    }

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, LOW, 5);

    }

  }

}

////////////////////////////////////////////////////////////Effect 16

void effect_16(int effectcount) {
Serial.println("Effect 16");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      i = i + 1;

      registersWrite(i, HIGH, 5);

      delay(50);

    }

    clearLed(20);

    for (int i = (number_of_74hc595s * 8)+1; i >= 0; i--) {

      int  j = i * 2;

      registersWrite(j, HIGH, 5);

      delay(50);

    }

    for (int i = (number_of_74hc595s * 8)+1; i >= 0; i--) {

      int  j = i * 2;

      registersWrite(j, LOW, 5);

      delay(50);

    }

  }

}



////////////////////////////////////////////////////////////Effect 17

void effect_17(int effectcount) {
Serial.println("Effect 17");

  for (int i = 1; i <= effectcount; i++) {

    unsigned int leds = 0B01010101;

    // turnOutputsOff();

    digitalWrite(latchPin, LOW);
    for(byte j = 0 ; j < number_of_74hc595s; j++){
        shiftOut(dataPin, clockPin, MSBFIRST, leds);}
    digitalWrite(latchPin, HIGH);

    delay(500);

    // analogWrite ( outputEnable, 255 );

    for ( byte bright = 255; bright > 0; bright -= 5 ){

    //   analogWrite ( outputEnable, bright );

      delay ( 50 );

    }

    // analogWrite ( outputEnable, 0 );

    for ( byte bright = 0; bright < 255; bright += 5 ){

    //   analogWrite ( outputEnable, bright );

      delay ( 50 );

    }

  }

}

////////////////////////////////////////////////////////////Effect 18

void effect_18(int effectcount) {
Serial.println("Effect 18");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 5);

      delay(10);

    }

    for (int i = number_of_74hc595s * 8; i > 3; i--) {

      registersWrite(i, LOW, 5);

    }

  }

}

////////////////////////////////////////////////////////////

void updateShiftRegister(unsigned int leds) {

  byte lowLED  = lowByte(leds);

  byte highLED = highByte(leds);

  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, highLED);

  shiftOut(dataPin, clockPin, MSBFIRST, lowLED);

  digitalWrite(latchPin, HIGH);

}





/////////////////////////////////////////////////////////////

void clearLed(int clearSpeed) {

  for (int i = 0; i < number_of_74hc595s * 8; i++) {

    registersWrite(i, LOW, 0);

    delay(clearSpeed);

  }

}

////////////////////////////////////////////////////////////

void turnOutputsOn() {

//   digitalWrite(outputEnable, LOW);

}

void turnOutputsOff() {

//   digitalWrite(outputEnable, HIGH, 5);

}

////////////////////////////////////////////////////////////

void registersWrite(int index, int value, int time) {

  digitalWrite(latchPin, LOW);

  turnOutputsOn();

  for (int i = numOfRegisterPins - 1; i >=  0; i--) {

    digitalWrite(clockPin, LOW);

    int val = registers[i];

    digitalWrite(dataPin, val);

    digitalWrite(clockPin, HIGH);

  }

  digitalWrite(latchPin, HIGH);

  registers[index] = value;

  delay(time);
}
#endif//18 Effect 
#ifdef test
byte dataLed[24][2] = {
  0B00000000,0B00000000,
  0B00000000,0B00000001,
  0B00000000,0B00000011,
  0B00000000,0B00000111,
  0B00000000,0B00001111,
  0B00000000,0B00011111,
  0B00000000,0B00111111,
  0B00000000,0B01111111,
  0B00000000,0B11111111,
  0B00000001,0B11111110,
  0B00000011,0B11111100,
  0B00000111,0B11111000,
  0B00001111,0B11110000,
  0B00011111,0B11100000,
  0B00111111,0B11000000,
  0B01111111,0B10000000,
  0B11111111,0B00000000,
  0B11111110,0B00000000,
  0B11111100,0B00000000,
  0B11111000,0B00000000,
  0B11110000,0B00000000,
  0B11100000,0B00000000,
  0B11000000,0B00000000,
  0B10000000,0B00000000
};

int clockPin = 27; 

int latchPin = 14;  

int dataPin = 12;   

int outputEnablePin = 5;  

byte leds = 0;    

void setup(){

  pinMode(latchPin, OUTPUT);

  pinMode(dataPin, OUTPUT);  

  pinMode(clockPin, OUTPUT);

  pinMode(outputEnablePin, OUTPUT); 

}
int counter = 1000;
void loop() {


  for (byte i = 24; i > 0; i--){
//    while(counter > 0){counter--;
//    for (byte b = 0; b < 8; b--){
  
      updateShiftRegister(i);
  
      delay(500);
      
//      updateShiftRegister0();
//    }
//    }
    counter = 1000;
//  delay(500);
  }

}

void updateShiftRegister(byte i){

   digitalWrite(latchPin, LOW);

   shiftOut(dataPin, clockPin, LSBFIRST, dataLed[0][i]);
   shiftOut(dataPin, clockPin, LSBFIRST, dataLed[1][i]);

   digitalWrite(latchPin, HIGH);


   digitalWrite(latchPin, LOW);

}

void updateShiftRegister0(){

   digitalWrite(latchPin, LOW);

   shiftOut(dataPin, clockPin, LSBFIRST, dataLed[0][0]);
   shiftOut(dataPin, clockPin, LSBFIRST, dataLed[1][0]);

   digitalWrite(latchPin, HIGH);

}
#endif//
