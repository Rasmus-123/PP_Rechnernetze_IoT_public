/*Based on  Example code for HC-SR501 PIR motion sensor with Arduino. More info: www.www.makerguides.com */

/*
* PIR Bewegungssensor: Erfasst Veränderungen der Wärmestrahlung im Messbereich. Menschen strahlen Wärme ab und werden so erfasst. Wird eine Bewegung ermittelt, 
* erfolgt eine Nachricht (JSON) via MQTT an den Raspberry Pi. Die Bewegung wird mit Zeitpunkt in der Datenbank gespeichert.

* Wichtig: Vor Beginn der Messung benötigt der Sensor ein bis zwei Minuten zum Kalibrieren. Kalibrierung beginnt sobald der Sensor Strom hat und darf nicht 
* durch Bewegungen gestört werden, ansonsten kommt es zu Messfehlern
*/




#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>

#define pirPin 34 //TTGO Input Pin

//Netzwerk des Raspberry Pi
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";



int val = 0;
bool motionState = false; // Initial: keine Bewegung
int p = 100; //counter for ping
uint32_t chipID;
uint32_t sensorID = 501;

WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
  
  Serial.begin(115200);
  chipID = (ESP.getEfuseMac() << 40) >> 40; //ID für JSONmsg
  WiFi.setHostname("HCSR501");
  WiFi.begin(ssid, pass);
  pinMode(pirPin, INPUT);

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  //while (!client.connect("192.168.43.215", 1883)) {
  while (!client.connect("192.168.4.1", 1883)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  // initialisiert mqtt client
  mqtt.begin(client);

  Serial.print("connecting to mqtt broker...");
  while (!mqtt.connect("HCSR501", "public", "public")) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");  
}
void loop() {
  DynamicJsonDocument jsonDoc(512);
  char jsonMessageBuffer[100];
  jsonDoc["device-type"] = "HCSR501"; //Sensortype wird bei Benachrichtigung/ auf Website angezeigt
  jsonDoc["identifier"] = chipID + sensorID;
  p = p+1;
  //PIRPin auslesen
  val = digitalRead(pirPin);
  // Bei erfasster Bewegung wird eine JSONmsg per mqtt an den Raspberry Pi geschickt
  if (val == HIGH) {
    jsonDoc["msg-type"] = "Aktivitaet";
    jsonDoc["value"] = "high";
    Serial.println("Motion detected!");
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2",jsonMessageBuffer);
  }
  if (p >= 100) { 
    jsonDoc["msg-type"] = "ping";
    jsonDoc["value"] = "ok";
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2/ping", jsonMessageBuffer);
    p = 0;
  }
  // Delayerhöhung führt bei größere Bewegungen (durch Messbereich laufen) zu weniger Benachrichtigungen, bei kleinerern aber zur Nichterkennung
    delay(500);
  
}
