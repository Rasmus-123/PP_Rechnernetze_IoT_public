#include <WiFi.h> 
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <string>

// RasPi-WLAN 
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";

// Welche Triangulations-Ecke { rssi1, rssi2, rssi3}
#define NAME "rssi1"

// MAC Address - All Lowercase
std::string beaconMAC = "c1:66:8f:34:4f:b9"; // HPA2

WiFiClient client;
MQTTPubSubClient mqtt;

uint32_t chipID;

BLEScan* pBLEScan;
static BLEAdvertisedDevice* myDevice;

bool connected = false;
int ping = 100;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        std::string bluetoothName = advertisedDevice.getAddress().toString();
        
        if (bluetoothName == beaconMAC) // Compare MAC Addresses
        {
            // Scanvorgang beenden                        
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            connected = true;
        }
    }
};


void setup() 
{
    Serial.begin(115200);

    chipID = (ESP.getEfuseMac() << 40) >> 40;  //Sets unique ID for JSON-Message

    // Connect to WiFi-Network

    WiFi.setHostname(NAME);

    WiFi.begin(ssid, pass);

    Serial.print("connecting to wifi...");
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

    Serial.print("connecting to host...");
    while (!client.connect("192.168.4.1", 1883)) 
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

    // Initialize MQTT Client
    mqtt.begin(client);

    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect(NAME, "public", "public")) 
    {
        Serial.print(".");
        delay(1000);
    }

    mqtt.publish("/hello", "Triangulation " NAME " connected");
    Serial.println(" connected!");

    // BLE Scan Setup

    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); // Create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    Serial.println("Scanning...");
}

void loop()
{
    BLEScanResults foundDevices = pBLEScan->start(10); // Scan for 10 Seconds

    if (connected) 
    {
        DynamicJsonDocument jsonDoc(128);
        char jsonMessageBuffer[128];

        jsonDoc["device-type"] = "Triangulation";
        jsonDoc["identifier"] = chipID;

        ping++;
        delay(100);

        int rssi = myDevice->getRSSI(); 
        Serial.println("");
        Serial.print("RSSI:");
        Serial.println(rssi);

        jsonDoc["msg-type"] = "Triangulation";
        jsonDoc[NAME] = rssi;

        serializeJson(jsonDoc, jsonMessageBuffer);
        mqtt.publish("Rssi",jsonMessageBuffer);

        delay(200);
        if (p == 100) 
        {
            DynamicJsonDocument jsonPingDoc(128);
            char jsonPingMessageBuffer[128];

            jsonPingDoc["device-type"] = "Triangulation";
            jsonPingDoc["identifier"] = chipID;

            jsonPingDoc["msg-type"] = "ping";
            jsonPingDoc["value"] = "ok";

            serializeJson(jsonPingDoc, jsonPingMessageBuffer);
            mqtt.publish("ttgo2/ping", jsonPingMessageBuffer);

            p = 0;
        }

        // Clean up
        pBLEScan->clearResults();
        delete myDevice;
    }
}
