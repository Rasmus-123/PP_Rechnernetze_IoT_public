/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static BLEServer* pServer;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {};
    void onDisconnect(BLEServer* pServer) {
      pServer->startAdvertising(); // If the client is disconnected, 
                                   // restart the Advertising broadcast 
                                   // so that the client can search for itself again
    }
};

BLECharacteristic* pCharacteristic = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println();

  BLEDevice::init("ESP32-BLE-Server"); 
  pServer = BLEDevice::createServer(); // Create server
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);  // Create service
  pCharacteristic = pService->createCharacteristic(
                                                  CHARACTERISTIC_UUID, 
                                                  BLECharacteristic::PROPERTY_READ   |
                                                  BLECharacteristic::PROPERTY_WRITE  |
                                                  BLECharacteristic::PROPERTY_NOTIFY |
                                                  BLECharacteristic::PROPERTY_INDICATE
                                                  );
  pCharacteristic->setValue("RSSI");
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start(); 
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); // Broadcast the UUID of the service
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
}

unsigned long previousMillis = 0;
const long interval = 2000;

void loop() {
  unsigned long currentMillis = millis(); 
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    pCharacteristic->notify();  // Actively push data every interval
    
    /*std::string value = pCharacteristic->getValue();
    Serial.print("The new RSSI value is: ");
    Serial.println(value.c_str());
    delay(2000);*/
  }
}
