
/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>

// Respberry WLAN
const char* ssid = "iot-test";
const char* pass = "raspberrypi42";

WiFiClient client;
MQTTPubSubClient mqtt;
uint32_t chipID;
int rssi;
int p =0;


// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
static BLEClient* pClient;


#define FilterFaktor 0.2          //  running average Filter Einstellung
float val_f;                      //  0,05 erhöht die Verspätung bei Änderung, macht aber das Wert stabiler
int counter=1;

// run when connect and disconnect
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      //Serial.print("The characteristic value was: ");
      //Serial.println(value.c_str());
    }

    //if(pRemoteCharacteristic->canNotify())
      //pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);

  chipID = (ESP.getEfuseMac() << 40) >> 40;
  WiFi.setHostname("TTGO2");
  WiFi.begin(ssid, pass);

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  while (!client.connect("192.168.43.215", 1883)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  // initialize mqtt client
  mqtt.begin(client);

  Serial.print("connecting to mqtt broker...");
  while (!mqtt.connect("arduino", "public", "public")) {
      Serial.print(".");
      delay(1000);
  }
  mqtt.publish("/kuechentest", "connected");
  Serial.println(" connected!");
  
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
      Serial.print("Starte RSSI Meter für ");
      Serial.println(myDevice->getAddress().toString().c_str());
      
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {

    DynamicJsonDocument jsonDoc(512);
    char jsonMessageBuffer[100];
    jsonDoc["device-type"] = "TTGO2";
    // Unterschiedlich je Sensor vermutlich am Sinnvollsten. Also noch was anhängen oder so.
    jsonDoc["identifier"] = chipID;
    p = p+1; 
  
    delay(100);  // RSSI alle 100 mS abfragen

    // ----------------- Messwert Filter ------------------  
    val_f = pClient->getRssi() * FilterFaktor + val_f * (1 - FilterFaktor);
    
    if (!counter)        // Ausgabe jeder 5 durchlauf (2x pro Sekunde)
    {
      rssi = int(round(val_f)); // Runden und Ganze Zahl bildenv
      Serial.print("RSSI: ");
      Serial.println(rssi);
      counter = 5;

      /*if (rssi > 70)
      {
        jsonDoc["msg-type"] = "Aktivitaet";
        Serial.println("Tür benutzt!");
        serializeJson(jsonDoc, jsonMessageBuffer);
        mqtt.publish("ttgo2",jsonMessageBuffer);
        
      } 
      
      delay(700);
      if (p == 100) {
         jsonDoc["msg-type"] = "ping";
         if (rssi !=0){
           jsonDoc["value"] = "ok";
         } else {
           jsonDoc["value"] = "beeintraechtigt";
         }
         serializeJson(jsonDoc, jsonMessageBuffer);
         mqtt.publish("ttgo2/ping", jsonMessageBuffer);
         p = 0;
      }*/
    }
    else  counter--;
  }
} // End of loop
