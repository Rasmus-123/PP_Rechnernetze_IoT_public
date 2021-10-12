/*

 * Josephine Lechtermann
 * based on
      Rui Santos
      Complete project details at https://RandomNerdTutorials.com/esp32-hc-sr04-ultrasonic-arduino/

      Permission is hereby granted, free of charge, to any person obtaining a copy
      of this software and associated documentation files.

      The above copyright notice and this permission notice shall be included in all
      copies or substantial portions of the Software.
   and Arduino Example WiFiMQTT
   
 * Ultraschallsensor: Sensor sendet ein Ultraschallsignal aus. Trifft dieses auf eine Oberfläche wird es reflektiert. Anhand der Dauer bis die Schallwellen wieder auf den Sensor treffen wird die Distanz zur Oberfläche errechnet.
 * Anwendung: Sensor wird im Türrahmen auf Brusthöhe plaziert. 
 * Befindet sich das nächste Objekt in unter 40cm Distanz, wird die Nachricht an den Raspberry Pi geschickt, dass die Tür passiert wurde.
 */

#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>

//Connection to sensor
//Vcc = 5V
const int trigPin = 34; //Input Pin
const int echoPin = 32; //Output Pin

//Wifi
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";

long duration;
int distance;
int p =0;
uint32_t chipID;
WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  Serial.begin(115200);
  chipID = (ESP.getEfuseMac() << 40) >> 40; //Sets unique ID for JSON-message
  WiFi.setHostname("TTGO2");
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
  while (!mqtt.connect("arduino", "public", "public")) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");
}

void loop() {
  DynamicJsonDocument jsonDoc(512);
  char jsonMessageBuffer[100];
  jsonDoc["device-type"] = "Ultraschallsensor"; //Sensortype is shown as Name
  jsonDoc["identifier"] = chipID + 786586;
  p = p+1;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  //Distance 0 -> not normal function (cable disconnected, sensor blocked, ...)
  //if someone uses the door -> send an interpretable message to the Raspberrypi (=mqtt-broker)
  if (distance<40 && distance != 0){
    //nur ok?
    jsonDoc["value"] = distance;
    jsonDoc["msg-type"] = "Aktivitaet";
    Serial.println("Tür benutzt!");
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2",jsonMessageBuffer);
  } 
  //sends a ping and the information if the sensor seems to work correctly every X minutes
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
