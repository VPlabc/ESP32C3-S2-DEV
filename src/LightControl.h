#ifndef LIGHTCONTROL
#define LIGHTCONTROL

// int outputEnable = 5;

void turnOutputsOff();
void turnOutputsOn();
void registersWrite(int index, int value, int time);
void clearLed(int clearSpeed) ;
void updateShiftRegister(unsigned int leds);
void effect_18(int effectcount, int speed);
void effect_17(int effectcount, int speed);
void effect_16(int effectcount, int speed) ;
void effect_15(int effectcount, int speed);
void effect_14(int effectcount, int speed);
void effect_13(int effectcount, int speed) ;
void effect_12(int effectcount, int speed) ;
void effect_11(int effectcount, int speed);
void effect_10(int effectcount, int speed) ;
void effect_9(int effectcount, int speed) ;
void effect_8(int effectcount, int speed) ;
void effect_7(int effectcount, int speed);
void effect_6(int effectcount, int speed);
void effect_5(int effectcount, int speed);
void effect_4(int effectcount, int speed);
void effect_3(int effectcount, int speed);
void effect_2(int effectcount, int speed);
void effect_1(int effectcount, int speed);
void hieuung7();
void hieuung3();
void hieuung2();
void hieuung1();

void Light_loop(String dataLed);
void Light_setup();


#endif//LIGHTCONTROL
