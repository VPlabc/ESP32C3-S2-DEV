
#include "Arduino.h"
#include "LightControl.h"
#include <Arduino_JSON.h>
#include <assert.h>



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
9 gd3
10 gd4
23 clk
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
int clockPin = 21;
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

#define t1 2

#define t2 20

#define t3  10

//How many of the shift registers - change this

byte number_of_74hc595s = 1;



//do not touch

#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[8*32];

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

//   MySerial0.begin(9600);

  clearLed(0);

}

void Light_loop(String dataLed) {
    JSONVar myObject = JSON.parse(dataLed);
  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(myObject) == "undefined") {
    MySerial0.println("Parsing input failed!");
    return;
  }
  MySerial0.println(dataLed);
  int len =  myObject["Data"].length();
  for(int i = 0; i < len; i++) {
    // MySerial0.println("Effect: " + String((int) myObject["Data"][i][0]));
    MySerial0.print("Replay: " + String((int) myObject["Data"][i][3]));
    MySerial0.print("| speed: " + String((int) myObject["Data"][i][4]));
    MySerial0.println("| board amount: " + String((int) myObject["Data"][i][1]));
    number_of_74hc595s = (int) myObject["Data"][i][1];
    if((int) myObject["Data"][i][0] == 1){effect_1((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 2){effect_2((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 3){effect_3((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 4){effect_4((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 5){effect_5((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 6){effect_6((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 7){effect_7((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 8){effect_8((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 9){effect_9((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 10){effect_10((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 11){effect_11((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 12){effect_12((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 13){effect_13((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 14){effect_14((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 15){effect_15((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 16){effect_16((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 17){effect_17((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 18){effect_18((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    clearLed(0);
  }
  RTC_Get();
  

}

//////////////////////////////////////////////////////////// Effect 1
byte boiso = 3;
byte CountStep = 0;
void effect_1(int effectcount,int speed) {
MySerial0.println("Effect 1");
  for (int i = 1; i <= effectcount; i++) {CountStep = 0;
    while(CountStep < number_of_74hc595s * 8){
      
      for (int i = (boiso * CountStep) + CountStep; i < boiso * CountStep + 1; i++) {
        digitalWrite(latchPin, LOW);
        registersWrite(i, HIGH, 1*speed);
        delay(40*speed);
      }
      for (int i = boiso * CountStep + 1; i > (boiso * CountStep) + CountStep; i--) {
        registersWrite(i, HIGH, 1*speed);
        delay(40*speed);
      }
      CountStep ++;
      clearLed(10);
    }

  }

}

/////////////////////////////////////////////////////////////Effect 2

void effect_2(int effectcount,int speed) {
MySerial0.println("Effect 2");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i + 1, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i + 2, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i + 3, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i, LOW, 1*speed);

      delay(t1*speed);

      registersWrite(i + 1, LOW, 1*speed);

      delay(t1*speed);

    }

    clearLed(0);

    for (int i = (number_of_74hc595s * 8) - 1; i >= 0; i--) {

      registersWrite(i, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i - 1, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i - 2, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i - 3, HIGH, 1*speed);

      delay(t1*speed);

      registersWrite(i, LOW, 1*speed);

      delay(t1*speed);

      registersWrite(i - 1, LOW, 1*speed);

      delay(t1*speed);

    }

  }

}

////////////////////////////////////////////////////////Effect 3

void effect_3(int effectcount,int speed) {
MySerial0.println("Effect 3");

  for (int j = 1; j <= effectcount; j++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

      delay(10* speed);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, LOW, 1*speed);

      delay(10*speed);

    }

  }

}

//////////////////////////////////////////////////////////Effect 4

void effect_4(int effectcount,int speed) {
MySerial0.println("Effect 4");

  for (int j = 1; j <= effectcount; j++) {

    byte led = 0b10101010;

    digitalWrite(latchPin, LOW);
    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led);}

    digitalWrite(latchPin, HIGH);

    delay(20*speed);



    byte led1 = 0b01010101;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}

    digitalWrite(latchPin, HIGH);

    delay(20*speed);

  }

}

/////////////////////////////////////////////////////////Effect 5

void effect_5(int effectcount,int speed) {
MySerial0.println("Effect 5");

  for (int j = 1; j <= effectcount; j++) {

    byte led = 0b11111111;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led);}

    digitalWrite(latchPin, HIGH);

    delay(10*speed);
    byte led1 = 0b00000000;

    digitalWrite(latchPin, LOW);

    for(byte i = 0 ; i < number_of_74hc595s; i++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}

    digitalWrite(latchPin, HIGH);

    delay(10*speed);

  }

}

/////////////////////////////////////////////////////////Effect 6

void effect_6(int effectcount, int speed) {
MySerial0.println("Effect 6");

  for (int j = 1; j <= effectcount; j++) {

    int count = number_of_74hc595s * 8;

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      clearLed(0);

      registersWrite(i, HIGH, 0);  // chaser 1

      registersWrite(count, HIGH, 0); // chaser 2

      count--;
      delay(speed);
    }

  }

}

/////////////////////////////////////////////////////////////Effect 7

void effect_7(int effectcount, int speed) {
MySerial0.println("Effect 7");

  for (int i = 1; i <= effectcount; i++) {

    int count2 = number_of_74hc595s * 8;

    for (int j = 0; j < number_of_74hc595s * 8; j++) {

      clearLed(0);

      registersWrite(j, HIGH, 0);  // chaser 1

      registersWrite(j - 1, HIGH, 0);

      registersWrite(count2, HIGH, 0); // chaser 2

      registersWrite(count2 - 1, HIGH, 0); // chaser 2

      count2--;
      delay(speed);
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
void effect_8(int effectcount, int speed) {
MySerial0.println("Effect 8");

    byte led = 0B11111111;
    byte led1 = 0B00000000;
    
    digitalWrite(latchPin, LOW);
    for(byte a = 0 ; a < number_of_74hc595s; a++){
        shiftOut(dataPin, clockPin, MSBFIRST, led1);}
    digitalWrite(latchPin, HIGH);
    delay(2000);
  for (int i = 1; i <= effectcount; i++) {
    for(int b = 0 ; b < 10; b+=1){
        for(int i = 0 ; i < 60*speed; i++){   
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
        for(int i = 0 ; i < 40*speed; i++){   
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

void effect_9(int effectcount, int speed) {
MySerial0.println("Effect 9");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      for (int k = i; k < number_of_74hc595s * 8; k++) {

        registersWrite(k, HIGH, 1*speed);

        delay(2*speed);

        registersWrite(k, LOW, 1*speed);

        delay(2*speed);

      }

      registersWrite(i, HIGH, 1*speed);

      delay(4*speed);

    }

  }

}

//////////////////////////////////////////////////////////////////Effect 10

void effect_10(int effectcount, int speed) {
MySerial0.println("Effect 10");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = (number_of_74hc595s * 8) - 1; i >= 0; i--) {

      for (int k = 0; k < i; k++) {

        registersWrite(k, HIGH, 1*speed);

        delay(2*speed);

        registersWrite(k, LOW, 1*speed);

        delay(2*speed);

      }

      registersWrite(i, HIGH, 1*speed);

      delay(4*speed);

    }

  }

}

////////////////////////////////////////////////////////////Effect 11

void effect_11(int effectcount, int speed) {
MySerial0.println("Effect 11");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

      delay(6*speed);

      registersWrite(i + 2, LOW, 1*speed);

      delay(6*speed);

      registersWrite(i - 5, LOW, 1*speed);

    } clearLed(0);

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, HIGH, 1*speed);

      delay(6*speed);

      registersWrite(i - 2, LOW, 1*speed);

      delay(6*speed);

      registersWrite(i + 5, LOW, 1*speed);

    }

  }

}



///////////////////////////////////////////////////////////////Effect 12

void effect_12(int effectcount,int speed) {
MySerial0.println("Effect 12");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < (number_of_74hc595s * 8)/2; i++) {

      for (int k = i; k < (number_of_74hc595s * 8)/2; k++)

      {

        registersWrite(k, HIGH, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 1*speed);

        delay(20);

        registersWrite(k, LOW, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 1*speed);

        delay(20);

      }

      registersWrite(i, HIGH, 1*speed);

      registersWrite((number_of_74hc595s * 8)-1 - i, HIGH, 1*speed);

    }

  }

}

/////////////////////////////////////////////////////////////////Effect 13

void effect_13(int effectcount,int speed) {
MySerial0.println("Effect 13");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = ((number_of_74hc595s * 8)/2)-1; i >= 0; i--) {

      for (int k = 0; k <= i; k++)

      {

        registersWrite(k, HIGH, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 1*speed);

        delay(4*speed);

        registersWrite(k, LOW, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 1*speed);

        delay(4*speed);

      }

      registersWrite(i, HIGH, 1*speed);

      registersWrite(((number_of_74hc595s * 8)-1) - i, HIGH, 1*speed);

    }

  }

}

////////////////////////////////////////////////////////////Effect 14

void effect_14(int effectcount,int speed) {
MySerial0.println("Effect 14");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

      delay(t3*speed);

    }

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, LOW, 1*speed);

      delay(t3*speed);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, HIGH, 1*speed);

      delay(t3*speed);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, LOW, 1*speed);

      delay(t3*speed);

    }

  }

}

/////////////////////////////////////////////////////////////Effect 15

void effect_15(int effectcount,int speed) {
MySerial0.println("Effect 15");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

    }

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, LOW, 1*speed);

    }

  }

}

////////////////////////////////////////////////////////////Effect 16

void effect_16(int effectcount,int speed) {
MySerial0.println("Effect 16");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      i = i + 1;

      registersWrite(i, HIGH, 1*speed);

      delay(10*speed);

    }

    clearLed(4&speed);

    for (int i = (number_of_74hc595s * 8)+1; i >= 0; i--) {

      int  j = i * 2;

      registersWrite(j, HIGH, 1*speed);

      delay(10&speed);

    }

    for (int i = (number_of_74hc595s * 8)+1; i >= 0; i--) {

      int  j = i * 2;

      registersWrite(j, LOW, 1*speed);

      delay(10*speed);

    }

  }

}



////////////////////////////////////////////////////////////Effect 17

void effect_17(int effectcount,int speed) {
MySerial0.println("Effect 17");
    byte led = 0B10101010;
    byte led1 = 0B01010101;
  for (int i = 1; i <= effectcount; i++) {

    for (int i = 1; i <= 10; i++) {
      for(int b = 0 ; b < 10; b+=1){
          for(int i = 0 ; i < 40*speed; i++){   
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
          
      // digitalWrite(latchPin, LOW);
      // for(byte a = 0 ; a < number_of_74hc595s; a++){
      //     shiftOut(dataPin, clockPin, MSBFIRST, led);}
      // digitalWrite(latchPin, HIGH);
      // delay(2000);
      for(int b = 10 ; b > 0; b-=1){
          for(int i = 0 ; i < 40*speed; i++){   
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
}

////////////////////////////////////////////////////////////Effect 18

void effect_18(int effectcount,int speed) {
MySerial0.println("Effect 18");
    byte led = 0B11111111;
    byte led1 = 0B00000000;
  for (int i = 1; i <= effectcount; i++) {
        digitalWrite(latchPin, LOW);
      for(byte a = 0 ; a < number_of_74hc595s; a++){
          shiftOut(dataPin, clockPin, MSBFIRST, led);}
      digitalWrite(latchPin, HIGH);
      delay(10*speed);
        digitalWrite(latchPin, LOW);
      for(byte a = 0 ; a < number_of_74hc595s; a++){
          shiftOut(dataPin, clockPin, MSBFIRST, led1);}
      digitalWrite(latchPin, HIGH);
      delay(10*speed);
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

//   digitalWrite(outputEnable, HIGH, 5*speed);

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
