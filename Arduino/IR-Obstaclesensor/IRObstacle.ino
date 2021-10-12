/*
 * Josephine Lechtermann
 * based on Arduino Example Code WiFiMQTT
 * 
 * Code for IRObstaclesensor. Sensor should recognize if someone walkes through a door. Test: range too low (few centimeters)
 * Obstacle in 40cm isn't recognized -> not useful for this application
 * 
 * TTGO sends a JSON-Message via MQTT to Raspberry Pi. Activity is stored in the database
 */


#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>


const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";
int isObstaclePin = 4;  // Inputpin
int isObstacle = HIGH;  // HIGH: no obstacle
int p =0;
uint32_t chipID;

WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);
    chipID = (ESP.getEfuseMac() << 40) >> 40; //ID for JSON-message
    
    pinMode(isObstaclePin, INPUT);

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

    // initialize mqtt 
    mqtt.begin(client);
    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect("arduino", "public", "public")) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");
}

void loop() {
  DynamicJsonDocument jsonDoc(512);
  char jsonMessageBuffer[100];
  jsonDoc["device-type"] = "Obstacle"; //Sensortype is shown as Name (Website)
  // Unterschiedlich je Sensor
  jsonDoc["identifier"] = chipID;
  p = p+1;
  mqtt.update();
  isObstacle = digitalRead(isObstaclePin);

  //activity detection
  if (isObstacle == LOW){
    jsonDoc["value"] = "obstacle";
    jsonDoc["msg-type"] = "Aktivitaet";
    Serial.println("Aktivität!");
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2",jsonMessageBuffer);
    delay(1000);  
  } 

  //ping
  if (p == 1000) {
    jsonDoc["msg-type"] = "ping";
    if (distance !=0){
      jsonDoc["value"] = "ok";
    } else {
      jsonDoc["value"] = "beeintraechtigt";
    }
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2/ping", jsonMessageBuffer);
    p = 0;
  }
  delay(200);
}
