/*
 * Code für IRObstaclesensor. Ansatz: Sensor sollte dazu dienen festzustellen, ob eine Tür passiert wurde. Test: Reichweite liegt bei wenigen cm,
 * Unterbrechung in 40cm Distanz wird nie erkannt. Daher nicht für eine Tür geeignet. 
 * 
 * TTGO sendet bei Unterbrechung eine JSON-Message via MQTT an den Raspberry Pi. Die erfasste Aktvität wird in der Datenbank gespeichert, der Sensor erscheint auf der Website.
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

  //Aktivitätserkennung
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
