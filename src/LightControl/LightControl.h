#ifndef LIGHTCONTROL
#define LIGHTCONTROL

// int outputEnable = 5;

void turnOutputsOff();
void turnOutputsOn();
void registersWrite(int index, int value, int time);
void clearLed(int clearSpeed) ;
void updateShiftRegister(unsigned int leds);
void effect_18(int effectcount);
void effect_17(int effectcount);
void effect_16(int effectcount) ;
void effect_15(int effectcount);
void effect_14(int effectcount);
void effect_13(int effectcount) ;
void effect_12(int effectcount) ;
void effect_11(int effectcount);
void effect_10(int effectcount) ;
void effect_9(int effectcount) ;
void effect_8(int effectcount) ;
void effect_7(int effectcount);
void effect_6(int effectcount);
void effect_5(int effectcount);
void effect_4(int effectcount);
void effect_3(int effectcount);
void effect_2(int effectcount);
void effect_1(int effectcount);
void hieuung7();
void hieuung3();
void hieuung2();
void hieuung1();

void Light_loop();
void Light_setup();


#endif//LIGHTCONTROL