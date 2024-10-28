#include <Arduino.h>

#define EN_DEBUG

#define ESP32S2
// HardwareSerial MySerial0(0);
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

#ifndef Variable
#define Variable
class VariableG 
{
  public:
bool dimmer = false;
bool MeshEnb = false;
String RoleVar = "Node";
};
#endif//Variable 