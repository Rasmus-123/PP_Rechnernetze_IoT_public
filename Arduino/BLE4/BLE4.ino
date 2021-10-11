#include <WiFi.h> 
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>

#include <string>

// RasPi-WLAN
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";

#define NAME "rssi1"

//const char* beaconMAC = "d1:47:4c:ff:94:38"; //YJK4
std::string beaconMAC = "c1:66:8f:34:4f:b9"; // HPA2

WiFiClient client;

static BLEAdvertisedDevice* myDevice;

uint32_t chipID;


MQTTPubSubClient mqtt;

DynamicJsonDocument jsonDoc(512);
char jsonMessageBuffer[100];

BLEScan* pBLEScan;

int val;                  
bool connected = false;
int rssi;
int p = 0;

int scanTime = 10; //In seconds

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks{
    void onResult(BLEAdvertisedDevice advertisedDevice){
        //id.setData(advertisedDevice.getManufacturerData());

        std::string Bluetoothname = advertisedDevice.getAddress().toString();

        //Serial.println(Bluetoothname);
        
        if (Bluetoothname == beaconMAC){   // ibeacon Adresse Vergleichen (yjk4 -> d1:47:4c:ff:94:38)
            
            //Serial.println("- gefunden " + Bluetoothname);                 // wenn überwache Adresse gefunden wurde                                    
            BLEDevice::getScan()->stop();                               // Scanvorgang beenden
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            connected = true;
        }
    }
};


void setup() {

  Serial.begin(115200);
  chipID = (ESP.getEfuseMac() << 40) >> 40;  //Sets unique ID for JSON-message
  WiFi.setHostname(NAME);
  
  jsonDoc["device-type"] = "Triangulation";           //Sensortype is shown as Name
  
  // Unterschiedlich je Sensor vermutlich am sinnvollsten. Also noch was anhängen oder so.
  jsonDoc["identifier"] = chipID;
  
  WiFi.begin(ssid, pass);

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  while (!client.connect("192.168.4.1", 1883)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");
  
  mqtt.begin(client); // initialize mqtt client

  Serial.print("connecting to mqtt broker...");
  while (!mqtt.connect(NAME, "public", "public")) {
      Serial.print(".");
      delay(1000);
  }
  
  mqtt.publish("/hello", "Triangulation " NAME " connected");
  Serial.println(" connected!");

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  Serial.println("Scanning...");
  //pinMode(2, OUTPUT);
}

void loop(){

  BLEScanResults foundDevices = pBLEScan->start(scanTime);

  if (connected) {
    //p++;
    delay(100);
    
    rssi = myDevice->getRSSI(); 
    Serial.println("");
    Serial.print("RSSI:");
    Serial.println(rssi);

    jsonDoc["msg-type"] = "Biangulation";
    jsonDoc[NAME] = rssi; // "rssi1", "rssi2", "rssi3"
    
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("Rssi",jsonMessageBuffer);
 
    /*if (rssi < -80 && rssi > -110) {
      jsonDoc["msg-type"] = "Aktivitaet";
      jsonDoc["value"] = rssi;
    
      Serial.println("Tür benutzt!");
      serializeJson(jsonDoc, jsonMessageBuffer);
      mqtt.publish("ttgo2",jsonMessageBuffer);
    }
    
    delay(200);
    if (p == 100) {  //Erhöhen, wenn fertig getestet
         jsonDoc["msg-type"] = "ping";
         if (rssi>-110){
              jsonDoc["value"] = "anwesend";
         } else {
              jsonDoc["value"] = "abwesend";
         }
         serializeJson(jsonDoc, jsonMessageBuffer);
         mqtt.publish("ttgo2/ping", jsonMessageBuffer);
         p = 0;
    }*/
  
    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
    //delay(1000);
    delete myDevice;
  }
  
}
