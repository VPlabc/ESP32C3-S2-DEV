#include "MeshNetwork.h"
MeshNet meshNetwork;
#include "main.h"
main MeshMain;

int channelMes = 1;
 
// simulate temperature and humidity data
float t1 = 0;
float h1 = 0;

unsigned long currentMillisMes = millis();
unsigned long previousMillisMes = 0;   // Stores last time temperature was published

unsigned int readingIdMes = 0;   


 struct_message myData;  // data to send
 struct_message inDataMes;  // data received
//  structpairing pairingData;
 MessageType messageTypeMes ;
//  struct_setting messageSetting;

uint8_t defserverAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
PairingStatus pairingStatusMes  = NOT_PAIRED;
void MeshNet::SetParameter(byte Status){
if(Status == PAIR_REQUEST){pairingStatusMes  = PAIR_REQUEST;}
if(Status == PAIR_PAIRED){pairingStatusMes  = PAIR_PAIRED;}
}


void MeshNet::setInterval(long time){
  interval = time;
}
void MeshNet::getParameters(){
DataTrans datTrans;
dataTrans.currentMillis = currentMillisMes; 
dataTrans.previousMillis = previousMillisMes; 
//  datTrans.currentMillisPing = currentMillisPing;
//  datTrans.previousMillisMesPing = previousMillisMesPing;
//  datTrans.rssi_display = rssi_display;
//  meshNet.interval = interval;
//  dataTrans.sensors_saved = sensors_saved;
//  for(int i = 0; i < 6 ; i++){dataTrans.defserverAddress[i] = defserverAddress[i];}
}
// pairingStatusMes  autoPairing();

void promiscuous_rx_cbs(void *buf, wifi_promiscuous_pkt_type_t type) {
  // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  int rssi = ppkt->rx_ctrl.rssi;
  meshNet.rssi_display = rssi;
}

// void saveSensorDatas(byte RSSI,byte ID,byte type,byte maxMov,byte maxStat,byte inacT,byte distanceSta,byte energySta,byte distanceMov,byte energyMov);
// simulate temperature reading
float readDHTTemperature_Mesh() {
  t1 = random(0,40);
  return t1;
}

// simulate humidity reading
float readDHTHumidity_Mesh() {
  h1 = random(0,100);
  return h1;
}

void MeshNet::addPeer(const uint8_t * mac_addr, uint8_t chan){
  esp_now_peer_info_t peer;
  ESP_ERROR_CHECK(esp_wifi_set_channel(chan ,WIFI_SECOND_CHAN_NONE));
  esp_now_del_peer(mac_addr);
  memset(&peer, 0, sizeof(esp_now_peer_info_t));
  peer.channel = chan;
  peer.encrypt = false;
  memcpy(peer.peer_addr, mac_addr, sizeof(uint8_t[6]));
  if (esp_now_add_peer(&peer) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  memcpy(defserverAddress, mac_addr, sizeof(uint8_t[6]));
}

void MeshNet::printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

byte resentMes = 0;bool onceOnsent = true;
void OnDataSents(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if(status == ESP_NOW_SEND_FAIL){
    meshNet.interval = 1000;
    resentMes++;if(resentMes > 5){onceOnsent = true;
    resentMes = 0;Serial.println("fail 5 times");
    meshNet.SetParameter(PAIR_REQUEST);
    }
  }else{resentMes = 0;if(onceOnsent){meshNet.interval = 10000;onceOnsent = false;}}
}

byte repairMes = 0;
PairingStatus MeshNet::autoPairing(){
  switch(pairingStatusMes ) {
    case PAIR_REQUEST:
    Serial.print("Pairing request on channelMes "  );
    Serial.println(channelMes);

    // set WiFi channelMes   
    ESP_ERROR_CHECK(esp_wifi_set_channel(channelMes,  WIFI_SECOND_CHAN_NONE));
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
    }
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cbs);
    // set callback routines
    esp_now_register_send_cb(OnDataSents);
    // esp_now_register_recv_cb(MeshMain.OnDataRecvs());
    MeshMain.OnData();
    // set pairing data to send to the server
    structpairing.msgType = PAIRING;
    structpairing.id = BOARD_ID;     
    structpairing.channel = channelMes;

    // add peer and send request
    addPeer(defserverAddress, channelMes);
    esp_now_send(defserverAddress, (uint8_t *) &structpairing, sizeof(structpairing));
    previousMillisMes = millis();
    pairingStatusMes  = PAIR_REQUESTED;
    break;

    case PAIR_REQUESTED:
    // time out to allow receiving response from server
    currentMillisMes = millis();
    if(currentMillisMes - previousMillisMes > 250) {
      previousMillisMes = currentMillisMes;
      // time out expired,  try next channel
      channelMes ++;digitalWrite(LED, !(digitalRead(LED)));
      if (channelMes > MAX_CHANNEL){
         channelMes = 1;
         repairMes++;if(repairMes > 10){ESP.restart();}
      }   
      pairingStatusMes  = PAIR_REQUEST;
    }
    break;

    case PAIR_PAIRED:
      // nothing to do here 
    break;
  }
  return pairingStatusMes ;
}  


void MeshNet::setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(LED, OUTPUT);
  Serial.print("Client Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();


  #ifdef SAVE_channelMes 
    EEPROM.begin(10);
    lastchannelMes = EEPROM.read(0);
    Serial.println(lastChannel);
    if (lastchannelMes >= 1 && lastchannelMes <= MAX_CHANNEL) {
      channelMes = lastChannel; 
    }
    Serial.println(channel);
  #endif  
  pairingStatusMes  = PAIR_REQUEST;
}  
byte stepMes = 0;
void MeshNet::loop() {
  if (autoPairing() == PAIR_PAIRED) {
    unsigned long currentMillisMes = millis();
    if (currentMillisMes - previousMillisMes >= interval) {
      // Save the last time a new reading was published
      previousMillisMes = currentMillisMes;
      digitalWrite(LED, HIGH);
    // Serial.println("Paired!!!"  );
      //Set values to send
      if(stepMes > 0) {
        // if(sensors_saved  > 1) {
        //     interval = 1000;
        //     myData.rssi = sensors[stepMes-1].rssi;
        //     myData.msgType = sensors[stepMes-1].msgType;
        //     myData.id = sensors[stepMes-1].id;
        //     myData.distanceActive = sensors[stepMes-1].distanceActive;
        //     myData.energyActive = sensors[stepMes-1].energyActive;
        //     myData.distanceMoving = sensors[stepMes-1].distanceMoving;
        //     esp_err_t result = esp_now_send(defserverAddress, (uint8_t *) &myData, sizeof(myData));
        //   stepMes++;if(stepMes == sensors_saved){stepMes = -1;}
        // }
        // else{
            stepMes = 0;
            // }
      }
      if(stepMes == 0){
        myData.rssi = rssi_display;
        myData.msgType = DATA;
        myData.id = BOARD_ID;
        myData.distanceActive = readDHTTemperature_Mesh();
        myData.energyActive = readDHTHumidity_Mesh();
        myData.distanceMoving = readingIdMes++;
        esp_err_t result = esp_now_send(defserverAddress, (uint8_t *) &myData, sizeof(myData));
        ////////////////////////////////////////////////////////////////////////////////////////////////
        meshNet.currentMillisPing = millis();
        if(meshNet.currentMillisPing - meshNet.previousMillisPing > 15000) {
          // previousMillisPing = currentMillisPing;
          Serial.println("disconnect to gateway");
        }
        stepMes = 1;
      }
      if(stepMes < 0){stepMes = 0;}
      ////////////////////////////////////////////////////////////////////////////////////////////////
    }
  }
}