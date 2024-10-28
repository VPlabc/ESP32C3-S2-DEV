
#include "Arduino.h"
#include "LightControl.h"
#include <Arduino_JSON.h>
#include <assert.h>
#include "VarGlobal.h"
VariableG var;

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
int dataPin = 21;
int latchPin = 33;
int clockPin = 4;
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
#define ESP32S2
#define EN_DEBUG
#if defined(EN_DEBUG)
#ifdef ESP32S2
#define txPins 43
#define rxPins 44
// const int SerialBaudrate = 115200;  // or 19200 usually
// HardwareSerial MySerials(0);
#define debug Serial
#else
#define debug Serial
#endif//ESP32S2
#define DB(x) debug.print(x);
#define DBf(x) debug.printf(x);
#define DB_LN(x) debug.println(x);
#ifdef ESP32S2
// #define DB_BG(x) {debug.begin(SerialBaudrate, SERIAL_8N1, rxPins, txPins);};
#define DB_BG(x) debug.begin(x);
#else
#define DB_BG(x) debug.begin(x);
#endif//ESP32S2
#define DB_FL() debug.flush();

#else
#define DB_BG(...)
#define DB(...)
#define DB_LN(...)
#endif

#define t1 2

#define t2 20

#define t3  10

//How many of the shift registers - change this

int number_of_74hc595s = 1;



//do not touch

byte numOfRegisterPins = number_of_74hc595s * 8;

boolean registers[8*32];
uint8_t registersLED[128];

bool mappingRevert[10] = {1,1,0,0,0,0,0,0,0,0};
byte mappingPort[10] = {0,1,3,2,0,0,0,0,0,0};
byte Chip = 0;

void RTC_Get(){
  // get the current time
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    static uint8_t last_second = 0;
    if (last_second != now.second)
    {
        last_second = now.second;

        DB("20");
        DB(now.year);    // 00-99
        DB('-');
        if (now.month < 10) DB('0');
        DB(now.month);   // 01-12
        DB('-');
        if (now.day < 10) DB('0');
        DB(now.day);     // 01-31
        DB(' ');
        DB(WeekDays[now.dow - 1]); // 1-7
        DB(' ');
        if (now.hour < 10) DB('0');
        DB(now.hour);    // 00-23
        DB(':');
        if (now.minute < 10) DB('0');
        DB(now.minute);  // 00-59
        DB(':');
        if (now.second < 10) DB('0');
        DB(now.second);  // 00-59
        DB_LN();
    }
}
void Light_setup() {

  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
    // MySerial0.begin(SerialBaudrate, SERIAL_8N1, rxPin, txPin);
    DB_BG(115200);
    DB_LN("MySerial0");

        // initialize the RTC
    rtc.init();

    // test if clock is halted and set a date-time (see example 2) to start it
    if (rtc.isHalted())
    {
        DB_LN("RTC is halted. Setting time...");

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

  

}
void Effects(String dataLed){
    JSONVar myObject = JSON.parse(dataLed);
  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(myObject) == "undefined") {DB_LN("Parsing input failed!");return;}
  // DB_LN(dataLed);
  int len =  myObject["Data"].length();
  for(int i = 0; i < len; i++) {
    // DB_LN("Effect: " + String((int) myObject["Data"][i][0]));
    DB("Group: " + String((int) myObject["Data"][5]) + " - " + String((int) myObject["Data"][6]));
    DB(" | Replay: " + String((int) myObject["Data"][i][3]));
    DB("| speed: " + String((int) myObject["Data"][i][4]));
    DB_LN("| board amount: " + String((int) myObject["Data"][i][1]));
    for(int i = 0; i < (int) myObject["Map"].length(); i++){mappingPort[i] = ((int) myObject["Map"][i])-1;}
    for(int i = 0; i < (int) myObject["Revert"].length(); i++){mappingRevert[i] = ((int) myObject["Revert"][i]);}
    number_of_74hc595s = (int) myObject["Data"][i][1];
    numOfRegisterPins = number_of_74hc595s * 8;
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
    if((int) myObject["Data"][i][0] == 19){effect_19((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 20){effect_20((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    if((int) myObject["Data"][i][0] == 21){effect_21((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    // if((int) myObject["Data"][i][0] == 22){effect_18((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    // if((int) myObject["Data"][i][0] == 23){effect_18((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    // if((int) myObject["Data"][i][0] == 24){effect_24((int) myObject["Data"][i][3],(int) myObject["Data"][i][4]);}
    clearLed(0);
  }
  RTC_Get();
}
//////////////////////////////////////////////////////////// Effect 1
byte boiso = 3;
byte CountStep = 0;
void effect_1(int effectcount,int speed) {
DB_LN("Effect 1");
  for (int i = 1; i <= effectcount; i++) {CountStep = 0;
    while(CountStep < number_of_74hc595s * 8){
      
      for (int i = (boiso * CountStep) + CountStep; i < boiso * CountStep + 1; i++) {
        // digitalWrite(latchPin, LOW);
        registersWrite(i, HIGH, 1*speed);
        delay(40*speed);
      }
      for (int i = boiso * CountStep + 1; i > (boiso * CountStep) + CountStep; i--) {
        // registersWrite(i, HIGH, 1*speed);
        registersWrite(i, LOW, 1*speed);
        delay(40*speed);
      }
      CountStep ++;
      clearLed(10);
    }

  }

}

/////////////////////////////////////////////////////////////Effect 2

void effect_2(int effectcount,int speed) {
DB_LN("Effect 2");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 0);

      delay(t1*speed);

      registersWrite(i + 1, HIGH, 0);

      delay(t1*speed);

      registersWrite(i + 2, HIGH, 0);

      delay(t1*speed);

      registersWrite(i + 3, HIGH, 0);

      delay(t1*speed);

      registersWrite(i, LOW, 0);

      delay(t1*speed);

      registersWrite(i + 1, LOW, 0);

      delay(t1*speed);

    }

    clearLed(0);

    for (int i = (number_of_74hc595s * 8) - 1; i >= 0; i--) {

      registersWrite(i, HIGH, 0);

      delay(t1*speed);

      registersWrite(i - 1, HIGH, 0);

      delay(t1*speed);

      registersWrite(i - 2, HIGH, 0);

      delay(t1*speed);

      registersWrite(i - 3, HIGH, 0);

      delay(t1*speed);

      registersWrite(i, LOW, 0);

      delay(t1*speed);

      registersWrite(i - 1, LOW, 0);

      delay(t1*speed);

    }

  }

}

////////////////////////////////////////////////////////Effect 3

void effect_3(int effectcount,int speed) {
DB_LN("Effect 3");

  for (int j = 1; j <= effectcount; j++) {

    for (int i = 0; i < number_of_74hc595s * 8; i++) {

      registersWrite(i, HIGH, 1*speed);

      delay(1* speed);

    }

    for (int i = (number_of_74hc595s * 8)-1; i >= 0; i--) {

      registersWrite(i, LOW, 1*speed);

      delay(1*speed);

    }

  }

}

//////////////////////////////////////////////////////////Effect 4

void effect_4(int effectcount,int speed) {
DB_LN("Effect 4");

  for (int j = 1; j <= effectcount; j++) {
    for(byte i = 0 ; i < (number_of_74hc595s*8)/4; i++){
        // shiftOut(dataPin, clockPin, MSBFIRST, led);
      registersWrite(i*4, HIGH, 0);registersWrite((i*4)+1, LOW, 0);registersWrite((i*4)+2, LOW, 0);registersWrite((i*4)+3, LOW, 0);}

    delay(2*speed);
    for(byte i = 0 ; i < (number_of_74hc595s*8)/4; i++){
      registersWrite(i*4, LOW, 0);registersWrite((i*4)+1, HIGH, 0);registersWrite((i*4)+2, LOW, 0);registersWrite((i*4)+3, LOW, 0);}

    delay(2*speed);
    for(byte i = 0 ; i < (number_of_74hc595s*8)/4; i++){
      registersWrite(i*4, LOW, 0);registersWrite((i*4)+1, LOW, 0);registersWrite((i*4)+2, HIGH, 0);registersWrite((i*4)+3, LOW, 0);}

    delay(2*speed);
    for(byte i = 0 ; i < (number_of_74hc595s*8)/4; i++){
      registersWrite(i*4, LOW, 0);registersWrite((i*4)+1, LOW, 0);registersWrite((i*4)+2, LOW, 0);registersWrite((i*4)+3, HIGH, 0);}

    delay(2*speed);


  }

}

/////////////////////////////////////////////////////////Effect 5

void effect_5(int effectcount,int speed) {
DB_LN("Effect 5");

  for (int j = 1; j <= effectcount; j++) {

      for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, HIGH, 0);}

    delay(10*speed);

      for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 0);}

    delay(10*speed);

  }

}

/////////////////////////////////////////////////////////Effect 6

void effect_6(int effectcount, int speed) {
DB_LN("Effect 6");

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
DB_LN("Effect 7");

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

void effect_8(int effectcount, int speed) {
DB_LN("Effect 8");
    // supen();
    byte range = 100;
    byte low = 10;
    byte led = 0B11111111;
    byte led1 = 0B00000000;
  for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 0);}Push595();
    // delay(2000);
  for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, HIGH, 0);}
  for (int i = 1; i <= effectcount; i++) {
    for(int b = low ; b < range; b+=range/10){
        for(int i = 0 ; i < speed; i++){   
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds((b*range));
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led1);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds(((range-b)*range));
        }
    }    
    Push595();
    delay(2000);
    for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 0);}
    for(int b = range ; b > low; b-=range/10){
        for(int i = 0 ; i < 2*speed; i++){   
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds((b*range));
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led1);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds(((range-b)*range));
        }
    }
  Push595();

      // for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 0);Push595();}
  }
  
  //  resum();

}



//////////////////////////////////////////////////////////////Effect 9

void effect_9(int effectcount, int speed) {
DB_LN("Effect 9");

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
DB_LN("Effect 10");

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
DB_LN("Effect 11");

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
DB_LN("Effect 12");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = 0; i < (number_of_74hc595s * 8)/2; i++) {

      for (int k = i; k < (number_of_74hc595s * 8)/2; k++)

      {

        registersWrite(k, HIGH, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 1*speed);

        delay(10);

        registersWrite(k, LOW, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 1*speed);

        delay(10);

      }

      registersWrite(i, HIGH, 1*speed);

      registersWrite((number_of_74hc595s * 8)-1 - i, HIGH, 1*speed);

    }

  }

}

/////////////////////////////////////////////////////////////////Effect 13

void effect_13(int effectcount,int speed) {
DB_LN("Effect 13");

  for (int i = 1; i <= effectcount; i++) {

    for (int i = ((number_of_74hc595s * 8)/2)-1; i >= 0; i--) {

      for (int k = 0; k <= i; k++)

      {

        registersWrite(k, HIGH, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, HIGH, 1*speed);

        delay(2*speed);

        registersWrite(k, LOW, 1*speed);

        registersWrite(((number_of_74hc595s * 8)-1) - k, LOW, 1*speed);

        delay(2*speed);

      }

      registersWrite(i, HIGH, 1*speed);

      registersWrite(((number_of_74hc595s * 8)-1) - i, HIGH, 1*speed);

    }

  }

}

////////////////////////////////////////////////////////////Effect 14

void effect_14(int effectcount,int speed) {
DB_LN("Effect 14");

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
DB_LN("Effect 15");

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
DB_LN("Effect 16");

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
DB_LN("Effect 17");
    byte led = 0B10101010;
    byte led1 = 0B01010101;
    byte range = 100;
    byte low = 10;
  for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 0);}
  for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, HIGH, 10);Push595();}
    // delay(2000);
  for (int i = 1; i <= effectcount; i++) {
    for(int b = low ; b < range; b+=range/10){
        for(int i = 0 ; i < speed; i++){   
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds((b*range));
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led1);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds(((range-b)*range));
        }
    }
        
      // for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, HIGH, 0);}
    // delay(2000);
    for(int b = range ; b > low; b-=range/10){
        for(int i = 0 ; i < speed; i++){   
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds((b*range));
            digitalWrite(latchPin, LOW);
            for(byte a = 0 ; a < number_of_74hc595s; a++){
                shiftOut(dataPin, clockPin, MSBFIRST, led1);}
            digitalWrite(latchPin, HIGH);
            delayMicroseconds(((range-b)*range));
        }
    }


  }
      for (int k = 0; k < number_of_74hc595s * 8; k++) {registersWrite(k, LOW, 10);Push595();}
}

////////////////////////////////////////////////////////////Effect 18

void effect_18(int effectcount,int speed) {
DB_LN("Effect 18");
    byte led = 0B11111111;
    byte led1 = 0B00000000;
  for (int i = 1; i <= effectcount; i++) {
      for(byte a = 0 ; a < number_of_74hc595s*8; a++){
          registersWrite( led, HIGH, 0);}
      delay(10*speed);
      for(byte a = 0 ; a < number_of_74hc595s*8; a++){
          registersWrite( led, LOW, 0);}
      delay(10*speed);
  }

}

//////////////////////////////////////////////////////////Effect 19

void effect_19(int effectcount,int speed) {
DB_LN("Effect 19");

  for (int j = 1; j <= effectcount; j++) {
    for(byte i = 0 ; i < (number_of_74hc595s*8)/2; i++){
        // shiftOut(dataPin, clockPin, MSBFIRST, led);
      registersWrite(i*2, HIGH, 0);registersWrite((i*2)+1, LOW, 0);}

    delay(2*speed);
    for(byte i = 0 ; i < (number_of_74hc595s*8)/2; i++){
      registersWrite(i*2, LOW, 0);registersWrite((i*2)+1, HIGH, 0);

    delay(2*speed);

    }
  }

}

//////////////////////////////////////////////////////////Effect 20

void effect_20(int effectcount,int speed) {
DB_LN("Effect 20");

bool state[4];
  for (int j = 1; j <= effectcount; j++) {
    for(byte j = 0 ; j < 10; j++){
      for(byte o = 5 ; o > 0; o--){
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){
            if(j >= 0){registersWrite(i*16+k, LOW,0);}
          }
          for(byte k = 0 ; k < 4; k++){
            registersWrite(i*16+random(0,16), HIGH,0);
          }
      }
      delay(10);
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){
            registersWrite(i*16+k, LOW,0);
          }
      }
    }
  }

}
}

//////////////////////////////////////////////////////////Effect 21

void effect_21(int effectcount,int speed) {
DB_LN("Effect 21");
  for (int j = 1; j <= effectcount; j++) {
    for(byte j = 15 ; j > 0; j--){
      // for(byte o = 5 ; o > 0; o--){
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){registersWrite(i*16+k, LOW,0);}
          for(byte k = 0 ; k < 2; k++){
            if(j >= random(0,j)){registersWrite(i*16+random(0,j), HIGH,0);}else{registersWrite(i*16+k, LOW,0);}
          }
      }
      delay(10);
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){
            if(j >= 0){registersWrite(i*16+k, LOW,0);}
          }
      }
    // }
    // delay(5*speed);
  }
  }
}

//////////////////////////////////////////////////////////Effect 22

void effect_22(int effectcount,int speed) {
DB_LN("Effect 22");
  for (int j = 1; j <= effectcount; j++) {
    for(byte j = 15 ; j > 0; j--){
      for(byte o = 5 ; o > 0; o--){
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){registersWrite(i*16+k, LOW,0);}
          for(byte k = 0 ; k < 2; k++){
            if(j >= random(0,j)){registersWrite(i*16+random(0,j), HIGH,0);}else{registersWrite(i*16+k, LOW,0);}
          }
      }
      delay(10);
        for(byte i = 0 ; i < number_of_74hc595s/2; i++){
          for(byte k = 0 ; k < 16; k++){
            if(j >= 0){registersWrite(i*16+k, LOW,0);}
          }
      }
    }
    // delay(5*speed);
  }
  }
}

//////////////////////////////////////////////////////////Effect 23

void effect_23(int effectcount,int speed) {
DB_LN("Effect 23");
  for (int j = 1; j <= effectcount; j++) {
      for(byte i = 0 ; i < number_of_74hc595s*8; i++){registersWrite(i, HIGH,0); }
      delay(5*speed);
    // delay(5*speed);
  }
}

//////////////////////////////////////////////////////////Effect 24

void effect_24(int effectcount,int speed) {
DB_LN("Effect 24");
  for (int j = 1; j <= effectcount; j++) {
      for(byte i = 0 ; i < number_of_74hc595s*8; i++){registersWrite(i, LOW,0);}
      delay(5*speed);
    // delay(5*speed);
  }
}


#include "SupportFile/EncodeData.h"
////////////////////////////////////////////////////////////

void effect_0(int effectcount,int speed) {

}
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


void Push595(){

  digitalWrite(latchPin, LOW);
  for (int i = 0; i < number_of_74hc595s; i++) {
    Chip = mappingPort[i];
    if(mappingRevert[Chip]){shiftOut(dataPin, clockPin, MSBFIRST, registersLED[Chip]);}
    else{shiftOut(dataPin, clockPin, LSBFIRST, registersLED[Chip]);}
  }
  digitalWrite(latchPin, HIGH); 
  // delay(1);
  // delayMicroseconds(1000000);
}
////////////////////////////////////////////////////////////
int oldIndex = 0;
void registersWrite(int index, int value, int time) {
  registers[index] = value;
  for (int i = numOfRegisterPins - 1; i >=  0; i--) {
    registersLED[i] = EncodeRespondByte(registers[0+(i*8)],registers[1+(i*8)],registers[2+(i*8)],registers[3+(i*8)],registers[4+(i*8)],registers[5+(i*8)],registers[6+(i*8)],registers[7+(i*8)]);
  }
  Push595();
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
