//#03052024 update 
//auto pair Node to Node
//----------Gateway to node 
//----------------------------------------------------------------

#define Master
#define Slave
// #define RevertButtonState
// #define USEButton0
// #define USEbuttonAll

#define LED_STATUS 12
// #include "config_manager.h"
#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
#include <ArduinoJson.h>
#include <esp_wifi.h>
#include <EEPROM.h>
#ifdef RTC_Onl
#include "System/RTC.h"
RTCTime rtcTime;
#endif//RTC_Onl
// #include "Sensor/Button.h"
#include "SupportFile/EncodeData.h"
#include "VarGlobal.h"
VariableG MeshVar;


#if defined(USEButton0) || defined(USEButtonAll)
#include <ClickButton.h>
#endif//USEButton

#ifdef USEButton0
#ifdef RevertButtonState
  ClickButton button0(InPut0, LOW, CLICKBTN_PULLUP);
  #else
  ClickButton button0(InPut0, HIGH, LOW);
#endif//RevertButtonState
#endif//USEButton0
#ifdef USEbuttonAll
#ifdef RevertButtonState
  
  ClickButton button1(InPut1, HIGH, LOW);
  ClickButton button2(InPut2, HIGH, LOW);
  ClickButton button3(InPut3, HIGH, LOW);
  ClickButton button4(InPut4, HIGH, LOW);
  #else
  ClickButton button1(InPut1, LOW, HIGH);
  ClickButton button2(InPut2, LOW, HIGH);
  ClickButton button3(InPut3, LOW, HIGH);
  ClickButton button4(InPut4, LOW, HIGH);
#endif//RevertButtonState
#endif//USEbuttonAll

byte function = 0;
void buttonSetup(){
#ifdef USEButton0
    button0.debounceTime   = 20;   // Debounce timer in ms
    button0.multiclickTime = 250;  // Time limit for multi clicks
    button0.longClickTime  = 1000; // Time until long clicks register
#endif//USEButton0
#ifdef USEbuttonAll
    button1.debounceTime   = 20;   // Debounce timer in ms
    button1.multiclickTime = 250;  // Time limit for multi clicks
    button1.longClickTime  = 1000; // Time until long clicks register
    
    button2.debounceTime   = 20;   // Debounce timer in ms
    button2.multiclickTime = 250;  // Time limit for multi clicks
    button2.longClickTime  = 1000; // Time until long clicks register
    
    button3.debounceTime   = 20;   // Debounce timer in ms
    button3.multiclickTime = 250;  // Time limit for multi clicks
    button3.longClickTime  = 1000; // Time until long clicks register
    
    button4.debounceTime   = 20;   // Debounce timer in ms
    button4.multiclickTime = 250;  // Time limit for multi clicks
    button4.longClickTime  = 1000; // Time until long clicks register
#endif//USEbuttonAll
}
// Replace with your network credentials (STATION)
// const char* ssid = "Hoang Vuong";
// const char* password = "91919191";

esp_now_peer_info_t slave;
int chan; 
 
enum MessageType {PAIRING, DATA, STARTPAIR, COMMAND};
MessageType messageType;

int counter = 0;

// Set your Board and Server ID 
#define BOARD_ID 2
#define NET_ID 2
#define MAX_CHANNEL 11  // for North America // 13 in Europe

uint8_t serverAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};



enum PairingStatus {NOT_PAIRED, PAIR_REQUEST, PAIR_REQUESTED, PAIR_PAIRED, WAITING,};
PairingStatus pairingStatus = WAITING;

#define DataLen 50
#define DataRadarLen 150

#ifdef SAVE_CHANNEL
  int lastChannel;
#endif  
uint8_t channel = 1;
bool onStatus = false; 
// simulate temperature and humidity data
byte t = 0;
byte h = 0;


unsigned long SentlastEventTime = millis();
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 5000;        // Interval at which to publish sensor readings
unsigned long start_s;                // used to measure Pairing time
unsigned int readingId = 0;   
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  uint8_t msgType;
  uint8_t id;
  uint8_t netid;
  uint8_t SetValue[DataLen];
  uint8_t GetValue[DataLen];
  unsigned int readingId;
} struct_message;


typedef struct struct_Booster_message {
    uint8_t msgType;
    uint8_t rssi; 
    uint8_t id;
    uint8_t state;
    uint8_t NodeId;
    float  VMT;
    float  AMT;
    float  VMain;
    float  AMain;
} struct_Booster_message;

typedef struct struct_radar_message {
  uint8_t msgType;
  uint8_t id;
  uint8_t netid;
  uint8_t RadarValue[DataRadarLen];
} struct_radar_message;

typedef struct struct_Cabinet_message {
  uint8_t msgType;
  uint8_t id;
  uint8_t netid;
  float Vmain;
  float Amain;
  float Vsolar;
  float Asolar;
  float GyroData;
} struct_Cabinet_message;

  uint8_t LocalSetValue[DataLen];
  uint8_t LocalGetValue[DataLen];
  uint8_t LocalRadarValue[DataRadarLen];
///////////////////////////////////// Pairing /////////////////////////////////////////////
typedef struct struct_pairing {       // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t netid;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;
 /// Struct pairing RF booster
 struct struct_pairingBooster {       // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
};
struct_pairingBooster structpairingBooster;
struct_Booster_message BoosterincommingMessage;
struct_Booster_message BoosteroutMessage;
///////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    boolean Bit0;
    boolean Bit1;
    boolean Bit2;
    boolean Bit3;
    boolean Bit4;
    boolean Bit5;
    boolean Bit6;
    boolean Bit7;
} struct_bit;
struct_bit structBit;
//Create 2 struct_message 
struct_message myData;  // data to send
struct_message inData;  // data received
struct_message incomingReadings;
struct_message outgoingSetpoints;
struct_pairing pairingData;
struct_radar_message inRadarData;
struct_radar_message RadarData;
struct_Cabinet_message FeedbackData;
bool updateNow = false;
bool AutoPair = false;
int AlreadyPairRetry = 0;
void MasterMeshsetup();
void SlaveMeshsetup();
void MasterMeshloop();
void SlaveMeshloop();
void SetDataToSend(byte id, byte Setstate);
void initESP_NOW(); 


void SlaveDataBooster(const uint8_t * mac_addrs, const uint8_t *incomingDatas,int len);
void SlavePairBooster(const uint8_t * mac_addrl, const uint8_t *incomingDatal , int len);

void StartPairBooster(){
    structpairingBooster.msgType = STARTPAIR;
    structpairingBooster.id = 0;
    esp_now_send(serverAddress, (uint8_t *) &structpairingBooster, sizeof(structpairingBooster));
}

void StartPair(){
    // DB_LN("Wifi Channel: "  + String(WiFi.channel()));
    // esp_wifi_set_channel(WiFi.channel(),  WIFI_SECOND_CHAN_NONE);

    pairingData.msgType = STARTPAIR;
    pairingData.id = 0;
    // structpairingBooster.msgType = STARTPAIR;
    // structpairingBooster.id = 0;
    // esp_now_send(NULL, (uint8_t *) &pairingData, sizeof(pairingData));
    esp_now_send(serverAddress, (uint8_t *) &pairingData, sizeof(pairingData));
}
void Meshsetup(){
    esp_err_t addStatus = esp_wifi_start();
    if (addStatus == ESP_OK) {DB_LN("esp_wifi_start success"); }
    else{DB_LN("esp_wifi_start failed");}
      
      // check_protocol();
      // // esp_wifi_set_protocol(current_wifi_interface, 3);
      // esp_wifi_set_protocol(current_wifi_interface, 7);
      // check_protocol();  
  buttonSetup();
#ifdef Slave
// RoleVar = "Node";
DB_LN();
DB_LN("Roles: " + RoleVar);DB_FL();
if(RoleVar == "Node"){DB_LN("Mesh Init For Slave");DB_FL();SlaveMeshsetup();}
#endif //Slave
#ifdef Master
// RoleVar = "Bridge";
if(RoleVar == "Bridge"){DB_LN("Mesh Init For Master");SlaveMeshsetup();/*SlaveMeshsetup();*/}
// SlaveMeshsetup();
  // DB_LN("Data length " + String(sizeof(myData)));
#endif// Master  
  
}
void Meshloop(){
  if(MeshVar.MeshEnb){
    #ifdef Slave
    // RoleVar = "Node";
    if(RoleVar == "Node") {SlaveMeshloop();}
    #endif ///Slave
    #ifdef Master
    // RoleVar = "Bridge";
      if(RoleVar == "Bridge") {MasterMeshloop();}
    #endif //Master
    #ifdef USEButton0
        button0.Update();
    #endif //USEButton0
    #ifdef USEbuttonAll
        button1.Update();
        button2.Update();
        button3.Update();
        button4.Update();
      if (button1.clicks != 0) function = button1.clicks;
      if (button2.clicks != 0) function = button2.clicks;
      if (button3.clicks != 0) function = button3.clicks;
      if (button4.clicks != 0) function = button4.clicks;
      #endif//USEbuttonAll
    //   // Toggle LED on single clicks
    #ifdef USEButton0
      if(button0.clicks != 0) function = button0.clicks;
      if(button0.clicks == 1){DB_LN("Single 0 click");function = 0;SetDataToSend(0,3);}
    #endif //USEButton0
    #ifdef USEbuttonAll
      if(button1.clicks == 1){DB_LN("Single 1 click");function = 0;SetDataToSend(0,3);}
      if(button2.clicks == 1){DB_LN("Single 2 click");function = 0;}
      if(button3.clicks == 1){DB_LN("Single 3 click");function = 0;}
      if(button4.clicks == 1){DB_LN("Single 4 click");function = 0;}
      if(button1.clicks == 2){DB_LN("Double 1 click");function = 0; 
        StartPair();
      }
    #endif//USEbuttonAll
    #ifdef USEButton0
      if(button0.clicks == 2){DB_LN("Single 0 click2");function = 0; 
        StartPair();
      }
    #endif //USEButton0
    #ifdef USEbuttonAll
      if(button1.clicks == 2){DB_LN("Single 1 click2");function = 0;
        // initESP_NOW();  
        pairingData.msgType = STARTPAIR;
        pairingData.id = 0;
        esp_now_send(serverAddress, (uint8_t *) &pairingData, sizeof(pairingData));
      }
      if(button1.clicks == 3){DB_LN("Single click3");function = 0;}//   
      if(function == -1 && button1.depressed == true){DB_LN("hold button");function = 0;}
      if(function == -2 && button1.depressed == true){DB_LN("hold button 2");function = 0;}
      if(function == -3 && button1.depressed == true){DB_LN("hold button 3");function = 0;}
    #endif//USEbuttonAll
}
}
  struct_bit DecodeBit(unsigned int _data)
    {
        struct_bit ret;
        ret.Bit0 = _data & B00000001;
        ret.Bit1 = _data & B00000010;
        ret.Bit2 = _data & B00000100;
        ret.Bit3 = _data & B00001000;
        ret.Bit4 = _data & B00010000;
        ret.Bit5 = _data & B00100000;
        ret.Bit6 = _data & B01000000;
        ret.Bit7 = _data & B10000000;

        return ret;
    }

#ifdef Master

bool state1 = false;
bool state2 = true;
bool state3 = false;
void SetDataToSend(byte id, byte data) {
  // DB_LN("MeshEnb :" + String(MeshEnb));
  outgoingSetpoints.msgType = DATA;
  outgoingSetpoints.id = 0;
  outgoingSetpoints.SetValue[0] = random(0, 16);
  outgoingSetpoints.SetValue[1] = random(0, 16);
  outgoingSetpoints.SetValue[2] = random(0, 16);
  outgoingSetpoints.SetValue[3] = random(0, 16);
  LocalSetValue[0] = outgoingSetpoints.SetValue[0];
  LocalSetValue[1] = outgoingSetpoints.SetValue[1];
  LocalSetValue[2] = outgoingSetpoints.SetValue[2];
  LocalSetValue[3] = outgoingSetpoints.SetValue[3];
  // outgoingSetpoints.GetValue  = random(0, 100);
  outgoingSetpoints.readingId = counter++;
    for(byte k=0;k<DataLen;k++){outgoingSetpoints.SetValue[k] = LocalSetValue[k];}
    for(byte k=0;k<DataLen;k++){outgoingSetpoints.GetValue[k] = LocalGetValue[k];}
    if(MeshVar.MeshEnb)esp_now_send(NULL, (uint8_t *) &outgoingSetpoints, sizeof(outgoingSetpoints));
}


// ---------------------------- esp_ now -------------------------
void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  DB(macStr);
}

bool addPeer(const uint8_t *peer_addr, byte chanels) {      // add pairing
  memset(&slave, 0, sizeof(slave));
  const esp_now_peer_info_t *peer = &slave;
  memcpy(slave.peer_addr, peer_addr, 6);
  
  slave.channel = chanels; // pick a channel
  slave.encrypt = 0; // no encryption
  // check if the peer exists
  bool exists = esp_now_is_peer_exist(slave.peer_addr);
  
  if (exists) {DB_LN("Already Paired");AlreadyPairRetry++;if(AlreadyPairRetry > 2){AlreadyPairRetry = 0;esp_now_del_peer(peer_addr);Meshsetup();}return true;}
  else {
    esp_err_t addStatus = esp_now_add_peer(peer);
    if (addStatus == ESP_OK) {DB_LN("Pair success");return true; }
    else{DB_LN("Pair failed");return false;}
  }
} 

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  DB("Last Packet Master Send Status: ");
  DB(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success to " : "Delivery Fail to ");
  printMAC(mac_addr);
  DB_LN();
  AutoPair = false;
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  DB(len);
  DB(" bytes of data received from : ");
  printMAC(mac_addr);
  DB_LN();
  StaticJsonDocument<1000> root;
  String payload;
  uint8_t type = incomingData[0];       // first message byte is the type of message 
  switch (type) {
  case DATA :   
  if(len == sizeof(incomingReadings)){ //light Data                       // the message is data type
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    // create a JSON document with received data and send it by event to the web page   
    // memcpy(LocalSetValue, incomingReadings.SetValue, sizeof(uint8_t[DataLen]));
    // memcpy(LocalGetValue, incomingReadings.GetValue, sizeof(uint8_t[DataLen]));
    for(byte k=0;k<DataLen;k++){LocalSetValue[k] = incomingReadings.SetValue[k];}
    for(byte k=0;k<DataLen;k++){LocalGetValue[k] = incomingReadings.GetValue[k];}
    root["id"] = incomingReadings.id;
    root["SetValue"] = LocalSetValue[incomingReadings.id] ;
    root["GetValue"] = LocalGetValue[incomingReadings.id] ;
    // DB("event send :");
    // serializeJson(root, Serial);
    // events.send(payload.c_str(), "new_readings", millis());
    // DB_LN();
    DB("ID  = ");
    DB_LN(inData.id);
    DB_LN("SetValue ");
    for(byte g = 0 ; g < 5 ; g++){
      if(g == 0){DB("[");}
      if(g == 0){ if (LocalSetValue[g] == 0){digitalWrite(LED_STATUS, LOW); } else { digitalWrite(LED_STATUS, HIGH);}}
        DB(LocalSetValue[g]);
      if(g == 0){DB("]");}
    DB("|");
    }
    DB_LN();
    DB_LN("GetValue ");
    for(byte g = 0 ; g < 5 ; g++){
      if(g == 0){DB("[");}
      if(g == 0){LocalGetValue[g] = digitalRead(LED_STATUS);}
        DB(LocalGetValue[g]);
      if(g == 0){DB("]");}
    DB("|");
    }
    DB_LN();
    DB("reading Id  = ");
    DB_LN(inData.readingId);
    serializeJson(root, payload);
  }//incoming data
  if(len == sizeof(BoosterincommingMessage)){
    SlaveDataBooster(mac_addr, incomingData, len);;
  }
    break;
  
  case PAIRING:   
  if(len == sizeof(pairingData)){                         // the message is a pairing request 
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    DB_LN("Type: " + String(pairingData.msgType));
    DB_LN("ID: " + String(pairingData.id));
    DB("Pairing request from: ");
    printMAC(mac_addr);
    DB_LN();
    DB_LN("Chanel: " + String(pairingData.channel));
    if (pairingData.id > 0 && sizeof(pairingData) == len) {     // do not replay to server itself
      if (pairingData.msgType == PAIRING) { 
        pairingData.id = 0;       // 0 is server
        // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
        WiFi.softAPmacAddress(pairingData.macAddr);   
        pairingData.channel = chan;
        DB_LN("send response");
        esp_now_send(mac_addr, (uint8_t *) &pairingData, sizeof(pairingData));
        addPeer(mac_addr, pairingData.channel);
      }  
    }  else{DB_LN("Unknown or data not matching");}  
  }
  if(len == sizeof(structpairingBooster)){
    SlavePairBooster(mac_addr, incomingData, len);
  }
    break; 
    
  case COMMAND:                            // the message is a pairing request 
    memcpy(&FeedbackData, incomingData, sizeof(FeedbackData));
    DB_LN("Type: " + String(FeedbackData.msgType));
    DB_LN("ID: " + String(FeedbackData.id));
    DB("Feedback request from: ");
    printMAC(mac_addr);
    DB_LN();
    if (FeedbackData.id > 0 && sizeof(FeedbackData) == len) {     // do not replay to server itself
      if (FeedbackData.msgType == COMMAND) { 
        FeedbackData.id = 0;       // 0 is server
        // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 

        DB_LN("send response");
        esp_now_send(mac_addr, (uint8_t *) &FeedbackData, sizeof(FeedbackData));
      }  
    }  else{DB_LN("Unknown or data not matching");}  
    break; 

  }
}
esp_now_peer_info_t MeshSlave;

 bool BoosteraAddPeer(const uint8_t *peer_addr) {      // add pairing
  memset(&MeshSlave, 0, sizeof(MeshSlave));
  const esp_now_peer_info_t *peer = &MeshSlave;
  memcpy(MeshSlave.peer_addr, peer_addr, 6);
  
  MeshSlave.channel = chan; // pick a channel
  MeshSlave.encrypt = 0; // no encryption
  // check if the peer exists
  bool exists = esp_now_is_peer_exist(MeshSlave.peer_addr);
  if (exists) {
    // MeshSlave already paired.
    DB_LN("Already Paired");//AlreadyNode++;
    // esp_wifi_set_protocol(current_wifi_interface, WIFI_PROTOCOL_LR);DB_LN("Set wifi LR");
    // esp_now_send(broadcastAddress, (uint8_t *) &MSpairingData, sizeof(MSpairingData));
    return true;
  }
  else {
    esp_err_t addStatus = esp_now_add_peer(peer);
    if (addStatus == ESP_OK) {
      // Pair success
      DB_LN("Pair success");
      return true;
    }
    else 
    {
      DB_LN("Pair failed");
      return false;
    }
  }

} 

void initESP_NOW(){
    // Init ESP-NOW
    
  DB("[initESP_NOW] >> Wi-Fi Channel: ");DB_LN(chan);
    esp_wifi_set_channel(chan,  WIFI_SECOND_CHAN_NONE);
    // ESP_ERROR_CHECK(esp_wifi_set_channel(chan,  WIFI_SECOND_CHAN_NONE));
    if (esp_now_init() != ESP_OK) {
      DB_LN("Error initializing ESP-NOW");
    }
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    addPeer(serverAddress, chan);

} 

void MasterMeshsetup() {
  // Initialize Serial Monitor
  // Serial.begin(115200);
  buttonSetup();

  DB_LN();
  DB("Server MAC Address:  ");
  DB_LN(WiFi.macAddress());

  // Set device as a Wi-Fi Station
  // WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.mode(WIFI_AP_STA);
  //   if(WifiMode == 2){DB_LN("AP STA mode");}else{DB_LN("STA mode");}
  //   DB_LN("SSID: " + ethernet.ssid + " |Pass: " + ethernet.pass);
  //   WiFi.begin(ethernet.ssid.c_str(), ethernet.pass.c_str());
    // esp_wifi_set_protocol(current_wifi_interface, WIFI_PROTOCOL_11B);
    
    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(500);DB_LN("Connecting to WiFi..");
    //   connectCounter++;if(connectCounter > Reconnect){connectCounter = Reconnect + 5;WifiMode = 1;break;}
    //     // Print ESP Local IP Address
    // }

  DB("Server SOFT AP MAC Address:  ");
  DB_LN(WiFi.softAPmacAddress());
  // esp_wifi_set_channel(WiFi.channel(), WIFI_SECOND_CHAN_NONE);
  chan = WiFi.channel();
  DB("Station IP Address: ");
  DB_LN(WiFi.localIP());
  initESP_NOW();
}
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
void MasterMeshloop() {

  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS ) {
    // events.send("ping",NULL,millis());
    lastEventTime = millis();
    if(AutoPair == true){
      initESP_NOW();
    }
    // memcpy(outgoingSetpoints.SetValue, LocalSetValue, sizeof(uint8_t[DataLen]));
    // memcpy(outgoingSetpoints.GetValue, LocalGetValue, sizeof(uint8_t[DataLen]));
  }

}
#endif//Master

#ifdef Slave


void SlaveData(const uint8_t * mac_addr, const uint8_t *incomingData , int len);
void SlavePair(const uint8_t * mac_addr, const uint8_t *incomingData , int len);

void SlaveaddPeer(const uint8_t * mac_addr, uint8_t chan){
  esp_now_peer_info_t peer;
  // ESP_ERROR_CHECK(esp_wifi_set_channel(WiFi.channel() ,WIFI_SECOND_CHAN_NONE));
  esp_wifi_set_channel(chan ,WIFI_SECOND_CHAN_NONE);
  esp_now_del_peer(mac_addr);
  memset(&peer, 0, sizeof(esp_now_peer_info_t));
  peer.channel = chan;
  peer.encrypt = false;
  memcpy(peer.peer_addr, mac_addr, sizeof(uint8_t[6]));
  if (esp_now_add_peer(&peer) != ESP_OK){
    DB_LN("Failed to add peer");DB_FL();
    return;
  }else{
    DB_LN("add peer OK");DB_FL();
  }
  // memcpy(serverAddress, mac_addr, sizeof(uint8_t[6]));
}

void SlaveprintMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  DB(macStr);
}

void SlaveOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  DB("\r\nLast Packet Slave Send Status:\t");
  DB_LN(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void SlaveOnDataRecv( const uint8_t * mac_addr , const uint8_t *incomingData, int len) { 
  // DB("Packet received from: ");
  // SlaveprintMAC(mac_addr);
  // DB_LN();
  // DB("data size = ");
  // DB_LN(sizeof(incomingData));
  uint8_t type = incomingData[0];
  switch (type) {
  case DATA :      // we received data from server
    SlaveData(mac_addr, incomingData, len);
    SlaveDataBooster(mac_addr, incomingData, len);
    break;
  case PAIRING:    // we received pairing data from server
    SlavePair(mac_addr, incomingData, len);
    SlavePairBooster(mac_addr, incomingData, len);
    break; 
  case STARTPAIR:    // we received pairing data from server
    if(len == sizeof(pairingData) && pairingData.id == 0){
      pairingStatus = PAIR_REQUEST;
    }
    break;
  } 
}

PairingStatus autoPairing(){
  switch(pairingStatus) {
    case PAIR_REQUEST:
    DB("Pairing request on channel "  );
    DB_LN(channel);

    // set WiFi channel   
    ESP_ERROR_CHECK(esp_wifi_set_channel(channel,  WIFI_SECOND_CHAN_NONE));
    if (esp_now_init() != ESP_OK) {
      DB_LN("Error initializing ESP-NOW");
    }

    // set callback routines
    esp_now_register_send_cb(SlaveOnDataSent);
    esp_now_register_recv_cb(SlaveOnDataRecv);
  
    // set pairing data to send to the server
    pairingData.msgType = PAIRING;
    pairingData.id = SettingData[21].toInt(); //BOARD_ID;     
    pairingData.channel = channel;

    // add peer and send request
    SlaveaddPeer(serverAddress, channel);
    esp_now_send(serverAddress, (uint8_t *) &pairingData, sizeof(pairingData));
    previousMillis = millis();
    pairingStatus = PAIR_REQUESTED;
    break;

    case PAIR_REQUESTED:
    // time out to allow receiving response from server
    currentMillis = millis();
    if(currentMillis - previousMillis > 250) {
      previousMillis = currentMillis;
      // time out expired,  try next channel
      channel ++;
      if (channel > MAX_CHANNEL){
         channel = 1;
      }   
      digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
      pairingStatus = PAIR_REQUEST;
    }
    break;

    case PAIR_PAIRED:
      // nothing to do here
    break;
  }
  return pairingStatus;
}  


void SlaveData(const uint8_t * mac_addrs, const uint8_t *incomingDatas,int len){
  memcpy(&inData, incomingDatas, sizeof(inData));
    DB_LN("size incomingDatas " + String(len));
    DB_LN("size inData " + String(sizeof(inData)));
if(len == sizeof(inData)){
    for(byte k=0;k<DataLen;k++){LocalSetValue[k] = inData.SetValue[k];}
    for(byte k=0;k<DataLen;k++){LocalGetValue[k] = inData.GetValue[k];}
    for(byte k=0;k<DataRadarLen;k++){LocalRadarValue[k] = inRadarData.RadarValue[k];}
    DB("ID  = ");
    DB_LN(inData.id);
    // DB_LN("SetValue ");
    for(byte g = 0 ; g < 5 ; g++){
      if(g == SettingData[21].toInt()){
         structBit = DecodeBit(LocalSetValue[g]);
        //  digitalWrite(RL1, structBit.Bit0);
        //  digitalWrite(RL2, structBit.Bit1);
        //  digitalWrite(RL3, structBit.Bit2);
        //  digitalWrite(RL4, structBit.Bit3);
        if(structBit.Bit0){RL1_On;}else{RL1_Off;}
        if(structBit.Bit1){RL2_On;}else{RL2_Off;}
        if(structBit.Bit2){RL3_On;}else{RL3_Off;}
        if(structBit.Bit3){RL4_On;}else{RL4_Off;}
      }

      if(g == SettingData[21].toInt()){DB("[");}
        DB(LocalSetValue[g]);
      if(g == SettingData[21].toInt()){DB("]");}
      DB("|");
    }
    
    DB_LN();
    // DB_LN("GetValue ");
    for(byte g = 0 ; g < 5 ; g++){
       if(g == SettingData[21].toInt()){
          #ifdef BOARD_4I4O2AER
          bool State5 = digitalRead(RL1);
          bool State6 = digitalRead(RL2);
          bool State7 = digitalRead(RL3);
          bool State8 = digitalRead(RL4);
          #endif//BOARD_4I4O2AER
          
          #ifdef BOARD_4I10O2AER
          bool State5 = State[4];
          bool State6 = State[5];
          bool State7 = State[6];
          bool State8 = State[7];
          #endif//BOARD_4I10O2AER
      LocalGetValue[g] = EncodeRespondByte(0,0,0,0,State8,State7,State6,State5);}//LocalGetValue[g] = digitalRead(LED_STATUS);}
     
      if(g == SettingData[21].toInt()){DB("[");}
        DB(LocalGetValue[g]);
      if(g == SettingData[21].toInt()){DB("]");}
      DB("|");
    }
    DB_LN();
    DB("B1: "+String(structBit.Bit0) + "|B2: " + String(structBit.Bit1) + "|B3: " + String(structBit.Bit2) + "|B4: " + String(structBit.Bit3));
    DB_LN("B5: "+String(structBit.Bit4) + "|B6: " + String(structBit.Bit5) + "|B7: " + String(structBit.Bit6) + "|B8: " + String(structBit.Bit7));

    // updateNow = true;
    }
}
void SlavePair(const uint8_t * mac_addrl, const uint8_t *incomingDatal , int len){
  memcpy(&pairingData, incomingDatal, sizeof(incomingDatal));
// 
    DB_LN("size incomingDatas " + String(len));
    DB_LN("size pairingData " + String(sizeof(pairingData)));
    if (pairingStatus != PAIR_PAIRED && pairingData.id == 0 && sizeof(pairingData) == len) {              // the message comes from server
      SlaveprintMAC(mac_addrl);
      DB_LN();
      DB("Pairing done for ");
      SlaveprintMAC(pairingData.macAddr);
      DB(" on channel " );
      DB(channel);    // channel used by the server
      DB(" in ");
      DB(millis()-start_s);
      DB_LN("ms");
      SlaveaddPeer(mac_addrl, channel); // add the server  to the peer list 
      #ifdef SAVE_CHANNEL
        lastChannel = pairingData.channel;
        EEPROM.write(0, pairingData.channel);
        EEPROM.commit();
      #endif  
      pairingStatus = PAIR_PAIRED;             // set the pairing status

      // pairingData.msgType = STARTPAIR;
      // pairingData.id = 0;
      // esp_now_send(serverAddress, (uint8_t *) &pairingData, sizeof(pairingData));
      
    }
    if (pairingStatus == PAIR_PAIRED) { 
      memcpy(&pairingData, incomingDatal, sizeof(pairingData));
      // DB_LN(pairingData.msgType);
      // DB_LN(pairingData.id);
      // DB("Pairing request from: ");
      // SlaveprintMAC(mac_addrl);
      // DB_LN();
      // DB_LN(pairingData.channel);
      // DB_LN("Net id: " + String(settings.MeshPort.toInt()));
      // DB_LN("Node id: " + String(pairingData.id));
      // DB_LN("Data len: " + String(len));
      // DB_LN("PairingData: " + String(sizeof(pairingData)));

      //  && settings.MeshPort.toInt() == pairingData.netid
      if (pairingData.id > 0 && sizeof(pairingData) == len) {     // do not replay to server itself
        if (pairingData.msgType == PAIRING) { 
          pairingData.id = 0;       // 0 is server
          // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
          WiFi.softAPmacAddress(pairingData.macAddr);   
          pairingData.channel = WiFi.channel();
          DB_LN("send response");
          esp_err_t result = esp_now_send(mac_addrl, (uint8_t *) &pairingData, sizeof(pairingData));
          SlaveaddPeer(mac_addrl, channel);
        }  
      }else{DB_LN("Unknown or data not matching");}  
    }
}

// Booster
void SlaveDataBooster(const uint8_t * mac_addrs, const uint8_t *incomingDatas,int len){
  StaticJsonDocument<1000> root;
  String payload;
  memcpy(&BoosterincommingMessage, incomingDatas, sizeof(BoosterincommingMessage));
    DB_LN("Booster Slave Data");
    // DB_LN("size incomingDatas " + String(len));
    // DB_LN("size BoosterincommingMessage " + String(sizeof(BoosterincommingMessage)));
    if(len == sizeof(BoosterincommingMessage)){
      // DB_LN("Type: DATA");
    if(BoosterincommingMessage.VMT > 10){BoosterincommingMessage.VMT = BoosterincommingMessage.VMT /10;}
    if(BoosterincommingMessage.VMain > 10){BoosterincommingMessage.VMain = BoosterincommingMessage.VMain /10;}
    if(BoosterincommingMessage.AMT > 10){BoosterincommingMessage.AMT = BoosterincommingMessage.AMT /10;}
    if(BoosterincommingMessage.AMain > 10){BoosterincommingMessage.AMain = BoosterincommingMessage.AMain /10;}
    root["id"] = String(BoosterincommingMessage.id);
    root["state"] = String(BoosterincommingMessage.state);
    root["rssi"] = String(BoosterincommingMessage.rssi);
    root["vmt"] = String(BoosterincommingMessage.VMT );
    root["amt"] = String(BoosterincommingMessage.AMT );
    root["vmain"] = String(BoosterincommingMessage.VMain );
    root["amain"] = String(BoosterincommingMessage.AMain );
    // CFrepondTime MeshRepondTime;
    // MeshRepondTime = CONFIG::Get_Time();
    // DB_LN("nows: " + String(MeshRepondTime.epochTime));
    // root["times"] = String(MeshRepondTime.epochTime);
    serializeJson(root, payload);
    Serial.print("event send :");
    serializeJson(root, Serial);

    // if(MeshconnectWebSocket == 1 || MeshconnectWebSocket == 2){socket_server->sendTXT(ESPCOM::current_socket_id, payload.c_str());}
    // events.send(payload.c_str(), "new_readings", millis());
    String DeviceTopic = "isoft/device" + String(BoosterincommingMessage.id);
      MQTTPush(DeviceTopic, payload);

    DB_LN();
    BoosteroutMessage.NodeId = BoosterincommingMessage.id;//recive done send callback to ID
    DataLogEnabled = true; 
    
    }
}
void SlavePairBooster(const uint8_t * mac_addrl, const uint8_t *incomingDatal , int len){
  memcpy(&structpairingBooster, incomingDatal, sizeof(incomingDatal));
    DB_LN("Booster Slave Pairing");
// 
    // DB_LN("size incomingDatas " + String(len));
    // DB_LN("size structpairingBooster " + String(sizeof(structpairingBooster)));
    if (pairingStatus != PAIR_PAIRED && structpairingBooster.id == 0 && sizeof(structpairingBooster) == len) {              // the message comes from server
      SlaveprintMAC(mac_addrl);
      DB_LN();
      DB("Pairing done for ");
      SlaveprintMAC(structpairingBooster.macAddr);
      DB(" on channel " );
      DB(channel);    // channel used by the server
      DB(" in ");
      DB(millis()-start_s);
      DB_LN("ms");
      SlaveaddPeer(mac_addrl, channel); // add the server  to the peer list 
      #ifdef SAVE_CHANNEL
        lastChannel = structpairingBooster.channel;
        EEPROM.write(0, structpairingBooster.channel);
        EEPROM.commit();
      #endif  
      pairingStatus = PAIR_PAIRED;             // set the pairing status

      // structpairingBooster.msgType = STARTPAIR;
      // structpairingBooster.id = 0;
      // esp_now_send(serverAddress, (uint8_t *) &structpairingBooster, sizeof(structpairingBooster));
      
    }
    if (pairingStatus == WAITING) { 
      memcpy(&structpairingBooster, incomingDatal, sizeof(structpairingBooster));
      // DB_LN(structpairingBooster.msgType);
      // DB_LN(structpairingBooster.id);
      // DB("Pairing request from: ");
      // SlaveprintMAC(mac_addrl);
      // DB_LN();
      // DB_LN(structpairingBooster.channel);
      // DB_LN("Net id: " + String(settings.MeshPort.toInt()));
      // DB_LN("Node id: " + String(structpairingBooster.id));
      // DB_LN("Data len: " + String(len));
      // DB_LN("structpairingBooster: " + String(sizeof(structpairingBooster)));

      //  && settings.MeshPort.toInt() == structpairingBooster.netid
      if (structpairingBooster.id > 0 && sizeof(structpairingBooster) == len) {     // do not replay to server itself
        if (structpairingBooster.msgType == PAIRING) { 
          if(RoleVar == "Node"){SlaveaddPeer(mac_addrl, channel);
          structpairingBooster.id = 0;       // 0 is server
          // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
          WiFi.softAPmacAddress(structpairingBooster.macAddr);   
          structpairingBooster.channel = WiFi.channel();
          DB_LN("send response");
          esp_err_t result = esp_now_send(mac_addrl, (uint8_t *) &structpairingBooster, sizeof(structpairingBooster));
          }
          if(RoleVar == "Bridge")if(addPeer(mac_addrl, WiFi.channel())){
            structpairingBooster.id = 0;       // 0 is server
            // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
            WiFi.softAPmacAddress(structpairingBooster.macAddr);   
            structpairingBooster.channel = WiFi.channel();
            DB_LN("send response");
            esp_now_send(mac_addrl, (uint8_t *) &structpairingBooster, sizeof(structpairingBooster));
          }
        }  
      }else{DB_LN("Unknown or data not matching");}  
    }
}
void SlaveMeshsetup() {
  // Serial.begin(115200);
  DB_LN();
  pinMode(LED_STATUS, OUTPUT);
  DB("Board MAC Address:  ");
  DB_LN(WiFi.macAddress());
  //
  // if(WiFi.status() == WL_CONNECTED){ WiFi.disconnect();WiFi.mode(WIFI_AP_STA);}
  start_s = millis();
  #ifdef SAVE_CHANNEL 
    EEPROM.begin(10);
    lastChannel = EEPROM.read(0);
    DB_LN(lastChannel);
    if (lastChannel >= 1 && lastChannel <= MAX_CHANNEL) {
      channel = lastChannel; 
    }
    DB_LN(channel);
  #endif  
  pairingStatus = WAITING;// PAIR_REQUEST;
  // SettingData[21] = String(BOARD_ID);
    if(RoleVar == "Bridge"){
    // WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    DB_LN("Wifi Channel: "  + String(WiFi.channel()));
    esp_wifi_set_channel(WiFi.channel(),  WIFI_SECOND_CHAN_NONE);

    }else{
    // WiFi.disconnect();
    WiFi.mode(WIFI_AP_STA);

  // set WiFi channel   
    DB_LN("Wifi Channel: "  + String(channel));
    esp_wifi_set_channel(channel,  WIFI_SECOND_CHAN_NONE);
    }
    if (esp_now_init() != ESP_OK) {
      DB_LN("Error initializing ESP-NOW");
    }

    // set callback routines
    esp_now_register_send_cb(SlaveOnDataSent);
    if(RoleVar == "Bridge"){
      esp_now_register_recv_cb(OnDataRecv);
    }else{
    esp_now_register_recv_cb(SlaveOnDataRecv);}
  
    if(RoleVar == "Bridge"){
    SlaveaddPeer(serverAddress, channel);
    }else{
    SlaveaddPeer(serverAddress, 1);
    }

    // add peer and send request
}  

static unsigned long currentMillis_a = millis();
void SlaveMeshloop() {
  if (autoPairing() == PAIR_PAIRED) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= random(1000, interval) || updateNow == true) {
      // Save the last time a new reading was published
      previousMillis = currentMillis;updateNow = false;
      //Set values to send
      myData.msgType = DATA;
      myData.id = SettingData[21].toInt();
      for(byte k=0;k<DataLen;k++){myData.SetValue[k] = LocalSetValue[k];}
      for(byte k=0;k<DataLen;k++){myData.GetValue[k] = LocalGetValue[k];}
      for(byte k=0;k<DataRadarLen;k++){RadarData.RadarValue[k] = LocalRadarValue[k];}
      myData.readingId = readingId++;
      esp_err_t result = esp_now_send(serverAddress, (uint8_t *) &myData, sizeof(myData));
      digitalWrite(LED_STATUS, LOW);
      onStatus = true; currentMillis_a = millis();
    }
    if(millis() - currentMillis_a > 100  && onStatus == true) {onStatus = false;
      currentMillis_a = millis();
      digitalWrite(LED_STATUS, HIGH);
    }
  }

  static const unsigned long EVENT_INTERVAL_MS = 1000;
  if ((millis() - SentlastEventTime) > EVENT_INTERVAL_MS) {
    if(BoosteroutMessage.NodeId > 0 ){
      BoosteroutMessage.msgType = DATA;
      DB_LN("Event interval");
    //   events.send("ping",NULL,millis());
      SentlastEventTime = millis();
    //   readDataToSend();
      esp_now_send(NULL, (uint8_t *) &BoosteroutMessage, sizeof(BoosteroutMessage));
      DB_LN("Sent to Node "  + String(BoosteroutMessage.NodeId));
      // Sentcount++;if(Sentcount>5){Sentcount = 0;BoosterincommingMessage.NodeId = 0;//SaveDataBool = 1;
      // }
      // if(Sentcount>3){//esp_wifi_set_protocol(current_wifi_interface, WIFI_PROTOCOL_LR);DB_LN("Set wifi LR");
      // }
      // if(SaveBackup++ > 0){SaveBackup = 0; SaveDataBool = 1;}
    }
  }

}

#endif//Slave