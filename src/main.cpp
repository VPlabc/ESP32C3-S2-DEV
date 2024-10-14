/*
#29/11/2023
#update: Auto reconnect mesh, send Radar data, firmware update form host
*/
//{"Application":[{"app":"0,0,7,0,0,Demo Machine1,0,2,1,3,4,5,6"},{"app":"1,0,7,1,0,Demo Machine2,7,9,8,10,11,12,13"},{"app":"2,0,7,2,0,Demo Machine3,14,16,15,17,18,19,20"},{"app":"3,0,7,3,0,Demo Machine4,21,23,22,24,25,26,27"}]}
//{"app":"0,0,7,0,0,Demo Machine 1,0,2,1,3,4,5,6"},
#include <Arduino.h>
// #define Rad
// #define Sendmessage
// #define BLE
// #define Websocket
// #define ESPNOW_Test
// #define Master
// #define Slave
// #define autoPair
// #define Printer
// #define SAVE_CHANNEL
// #define IOTDEVICES
#define LED_Controller

#ifdef LED_Controller

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


#include "SupportFile/StringSplit.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
// #include "PSRam.h"
// #include "esp_wifi.h"
//#include <ArduinoJson.h>

#define LOG(string) {Serial.print(string);} 
#define LOGLN(string) {Serial.println(string);}
// Replace with your network credentials
const char* ssid = "I-Soft";
const char* password = "i-soft@2023";

// Create AsyncWebServer object on port 80
// AsyncWebServer server(80);
JSONVar readings;
bool ledState = 0;
int printStatus = 0;
// Create a WebSocket object
AsyncWebSocket ws("/ws");
String getSensorReadings();
void initWiFi();
void initSPIFFS();
void initWebSocket();
void notifyClients(String sensorReadings);
void printScript();
void notifyClientsLed();
void initPSRAM();
String processor(const String& var);
String ConvBinUnits(int bytes, int resolution) ;


TaskHandle_t TskLED;
TaskHandle_t TskPortal;
TaskHandle_t Tsk595;

String dataLeds = "{\"a\":[\
  [1,3,2],\
  [2,5,2],\
  [3,5,2],\
  [4,5,2],\
  [5,10,10],\
  [6,10,10],\
  [7,10,10],\
  [8,10,2],\
  [9,5,2],\
  [10,5,2],\
  [11,5,2],\
  [12,5,2],\
  [13,10,2],\
  [14,10,2],\
  [15,10,2],\
  [16,10,2],\
  [5,5,5],\
  [6,5,5],\
  [7,5,5],\
  [8,10,2],\
  [9,5,2],\
  [10,5,2],\
  [11,5,2],\
  [12,5,2],\
  [13,5,2],\
  [14,5,2],\
  [15,5,2],\
  [16,5,2]\
]}";

String effectRead = "";
#include "LightControl.h"
//#include "ExFlash.h"
#include "FS_Funct.h"

#include "WifiPortTal.h"
// #include "FSBrowser.h"
// SHTC3 demo(BAUD_4800, 0x01);


// uint8_t current_protocol;
// esp_interface_t current_esp_interface;
// wifi_interface_t current_wifi_interface;
// int rssi_display;

// Estructuras para calcular los paquetes, el RSSI, etc
// typedef struct {
//   unsigned frame_ctrl: 16;
//   unsigned duration_id: 16;
//   uint8_t addr1[6]; /* receiver address */
//   uint8_t addr2[6]; /* sender address */
//   uint8_t addr3[6]; /* filtering address */
//   unsigned sequence_ctrl: 16;
//   uint8_t addr4[6]; /* optional */
// } wifi_ieee80211_mac_hdr_t;

// typedef struct {
//   wifi_ieee80211_mac_hdr_t hdr;
//   uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
// } wifi_ieee80211_packet_t;
// //La callback que hace la magia
// void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
//   // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
//   if (type != WIFI_PKT_MGMT)
//     return;

//   const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
//   const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
//   const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

//   int rssi = ppkt->rx_ctrl.rssi;
//   rssi_display = rssi;
// }
// bool LooklineDebug = true;
// int check_protocol()
// {
//     char error_buf1[100];
//   if(LooklineDebug){
//     DB_LN();
//     DB_LN("___________________________________");
//     DB_LN();
//   }
//      esp_err_t error_code = esp_wifi_get_protocol(current_wifi_interface, &current_protocol);
//      esp_err_to_name_r(error_code,error_buf1,100);
//   if(LooklineDebug){
//      DB("esp_wifi_get_protocol error code: ");
//      DB_LN(error_buf1);
//     DB_LN("Code: " + String(current_protocol));
//     if ((current_protocol&WIFI_PROTOCOL_11B) == WIFI_PROTOCOL_11B)
//       DB_LN("Protocol is WIFI_PROTOCOL_11B");
//     if ((current_protocol&WIFI_PROTOCOL_11G) == WIFI_PROTOCOL_11G)
//       DB_LN("Protocol is WIFI_PROTOCOL_11G");
//     if ((current_protocol&WIFI_PROTOCOL_11N) == WIFI_PROTOCOL_11N)
//       DB_LN("Protocol is WIFI_PROTOCOL_11N");
//     if ((current_protocol&WIFI_PROTOCOL_LR) == WIFI_PROTOCOL_LR)
//       DB_LN("Protocol is WIFI_PROTOCOL_LR");
//     DB_LN("___________________________________");
//     DB_LN();
//     DB_LN();
//     DB_FL();
//   }
//     return current_protocol;
// }

void TskWifi( void * pvParameters ) {
  DB("Task Wifi running on core ");
  DB_LN(xPortGetCoreID());
    for (;;) {
      Portal_loop();
    }
  }
void TskLEDControl( void * pvParameters ) {
  DB("Task LEDControl running on core ");
  DB_LN(xPortGetCoreID());

    for (;;) {
      Light_loop(effectRead);
      delay(500);
    }
  }

void TskPush595( void * pvParameters ) {
  DB("Task Push595 running on core ");
  DB_LN(xPortGetCoreID());
  long current_Time = millis();
  long time = 500;
  pinMode(1, OUTPUT);
    for (;;) {
      if(millis() - current_Time > time) {current_Time = millis();digitalWrite(1, !digitalRead(1));}
      Push595();

    }
  }
//#############################################################################################
String ConvBinUnits(int bytes, int resolution) {
  if      (bytes < 1024)                 {
    return String(bytes) + " B";
  }
  else if (bytes < 1024 * 1024)          {
    return String((bytes / 1024.0), resolution) + " KB";
  }
  else if (bytes < (1024 * 1024 * 1024)) {
    return String((bytes / 1024.0 / 1024.0), resolution) + " MB";
  }
  else return "";
}
void TaskInit(){
    //------------------------------------------------------------------------
  //TskWIFI : Led Control task
  xTaskCreatePinnedToCore(
    TskPush595,   /* Task function. */
    "Task Push595",     /* name of task. */
    800,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task*/
    &Tsk595,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0/1 */
  delay(500);
  //------------------------------------------------------------------------
  //TskWIFI : WiFi task
  xTaskCreatePinnedToCore(
    TskWifi,   /* Task function. */
    "Task Portal",     /* name of task. */
    5000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    2,           /* priority of the task*/
    &TskPortal,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0/1 */
  delay(500);
  //------------------------------------------------------------------------
  //TskWIFI : Led Control task
  xTaskCreatePinnedToCore(
    TskLEDControl,   /* Task function. */
    "Task LED",     /* name of task. */
    5000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task*/
    &TskLED,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0/1 */
  delay(500);

  
}
void setup(){
    DB_BG(115200);
    DB_LN("MySerial Main");


      // check_protocol();
      // // esp_wifi_set_protocol(current_wifi_interface, 3);
      // esp_wifi_set_protocol(current_wifi_interface, 3);
      // check_protocol();  


  // initWiFi();
//  initPSRAM();
  initSPIFFS();
  initWebSocket();
  // Web Server Root URL
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/index.html", "text/html");
  // });

  // server.serveStatic("/", SPIFFS, "/");

  // // Start server
  // server.begin();
  DB_FL();
  Portal_setup(1);
  // Browser_setup();
  DB_LN("________________________________________________________________");
  listDir(SPIFFS, "/", 0);
  DB_LN("________________________________________________________________");
    effectRead = readFile(SPIFFS, "/effect.js");
    DB_LN(effectRead);
  DB_FL();
  Light_setup();   
//  Flash_setup(); 
  TaskInit();
}

void loop(){
  // Browser_loop();
  static unsigned lastTimeSensor = millis();
  static unsigned timerDelaySensor = 2000;
  if ((millis() - lastTimeSensor) > timerDelaySensor) {
    //1 Start | 2 Ethernet | 3 socket connect | 4 socket disconnect | 5 Wifi STA | 6 Wifi AP
    DB("Current Free RAM: " + String(ConvBinUnits(ESP.getFreeHeap(), 1)) );
    DB_LN();

    lastTimeSensor = millis();
  }

  const long timerDelay = 1000;
  unsigned long lastTime = millis();
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
   DB(sensorReadings);
    notifyClients(sensorReadings);
    lastTime = millis();
  }

  ws.cleanupClients();

  static unsigned long previousMillis = 0;
  const long interval = 1000;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    printStatus = 0;     // get the current status of the ESCPOS printer
      ledState = !ledState;
      notifyClientsLed();
  }
    // delay(200);
  dnsServer.processNextRequest();

}
// Get Sensor Readings and return JSON object
String getSensorReadings(){
  if (printStatus == 0) {               // if status is 0 then we are good
    readings["Printer State"] = "printer online";   // debug that we are online
  } else {
    readings["Printer State"] = "printer offline: ";  // debug that we are offline
  //  DB_LN(printStatus);        // debug the returned status code  
  } 
  // readings["khoiluong"] = String(random(10,100)) + "Kg";
  // readings["Printer State"] = String(printStatus);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {DB_LN("An error has occurred while mounting SPIFFS");}
  else{DB_LN("SPIFFS mounted successfully");}
}
bool setup_mode = false;

void startAP(bool new_device) {
  // WiFi.mode(WIFI_AP);
  // WiFi.softAP("Wireless Serial", "12345678");
  // dnsServer.start(53, "*", WiFi.softAPIP());
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  // //more handlers...
  // server.begin();
  // setup_mode = true;
}

// void sendInfo(uint8_t num) {

//   byte mac_address[6];
//   WiFi.macAddress(mac_address);
//   StaticJsonDocument<150> info_data;
//   info_data["type"] = "info";
//   // info_data["version"] = gateway_settings.pversion;
//   info_data["wifi"] = String(WiFi.RSSI());
//   info_data["ip_address"] = WiFi.localIP().toString();
//   info_data["mac_address"] = WiFi.macAddress();
//   // info_data["version"] = FRMW_VERSION;
//   // info_data["baud"] = gateway_settings.baud;
//   char   b[150];
//   size_t len = serializeJson(info_data, b);  // serialize to buffer
//   if (num != 255) webSocket.sendTXT(num, b);
//   else webSocket.broadcastTXT(b, len);
// }
// Initialize WiFi
void initPSRAM(){
#ifdef PSRAMFS
PSRAM_setup();
#endif//PSRAMFS
}
void initWiFi() {
  WiFi.mode(WIFI_AP_STA);
  DB_LN("SSID: "+ String(ssid));
  DB_LN("PASS: "+ String(password));
  WiFi.begin(ssid, password);
 DB("Connecting to WiFi ..");
 byte wait = 0;
  while (WiFi.status() != WL_CONNECTED) {
   DB('.');
    delay(1000);
    if(wait++ >2){Portal_setup(0);delay(1000); break;}
  }
    //   byte tries = 1;
    // if ( String(ssid) != "" && String(password) != ""  ) {
    //   while (WiFi.status() != WL_CONNECTED ) {
    //     if (tries++ > 9 ) {
    //       startAP(false);
    //       break;
    //     }
    //     delay(1000);
    //   }
    // } else {
    //   startAP(true);
    // }
 DB_LN(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}
void notifyClientsLed() {
  readings["State"] = String(ledState);
  String jsonString = JSON.stringify(readings);
  ws.textAll(String(jsonString));
}

String processor(const String& var){
 DB_LN(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
      DB_LN((char*)data);
      JSONVar Obj = JSON.parse((char*)data);
      if (JSON.typeof(Obj) == "undefined") {
        DB_LN("Parsing input failed!");
        return;
      }
      DB_LN("CMD: " + String((const char*)Obj["CMD"]));
      if(String((const char*)Obj["CMD"]) == "Save"){
        writeFile(SPIFFS, "/effect.js", (char*)data);
        DB_LN((char*)data);
        effectRead = readFile(SPIFFS, "/effect.js");
        DB_LN(effectRead);
      }
      if(String((const char*)Obj["CMD"]) == "Try"){
        DB_LN("Try");
        effectRead = (char*)data;
        DB_LN(effectRead);
      }
      String sensorReadings = getSensorReadings();
      DB_LN(sensorReadings);
      notifyClients(sensorReadings);

  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
     Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
     Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
#endif//LED_Controller
