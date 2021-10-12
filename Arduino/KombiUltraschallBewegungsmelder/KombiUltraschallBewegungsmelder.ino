/**
 * Schnell kombiniert von Rasmus
 * 
 * Bewegungsmelder Based on  Example code for HC-SR501 PIR motion sensor with Arduino. More info: www.www.makerguides.com
 */

#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>

//Connection to sensor
//Vcc = 5V
const int trigPin = 34; //Input Pin
const int echoPin = 32; //Output Pin


const int pirPin = 36; //TTGO Input Pin

//Wifi
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";

long duration;
int distance;
int pu =0;
int pb =0;
int val = 0;
bool motionState = false; // We start with no motion detected.


uint32_t chipID;
uint32_t ultraschallID = 981213;
uint32_t bewegungsID = 141298;

WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  Serial.begin(115200);
  chipID = (ESP.getEfuseMac() << 40) >> 40; //Sets unique ID for JSON-message
  WiFi.setHostname("Kombi");
  WiFi.begin(ssid, pass);

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  //IP Adresse RasPi anpassen
  while (!client.connect("192.168.4.1", 1883)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");
  
  mqtt.begin(client); // initialize mqtt client

  Serial.print("connecting to mqtt broker...");
  while (!mqtt.connect("Kombi", "public", "public")) {
      Serial.print(".");
      delay(1000);
  }
  //mqtt.publish("/kuechentest", "connected");
  Serial.println(" connected!");
}

void ultraschall()
{
  DynamicJsonDocument jsonDoc(512);
  char jsonMessageBuffer[100];

  pu = pu+1;
  
  jsonDoc["device-type"] = "Ultraschallsensor"; //Sensortype is shown as Name
  // Unterschiedlich je Sensor vermutlich am Sinnvollsten. Also noch was anhängen oder so.
  jsonDoc["identifier"] = chipID + ultraschallID;
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
  if (distance<50 && distance != 0){
    //nur ok?
    jsonDoc["value"] = distance;
    jsonDoc["msg-type"] = "Aktivitaet";
    Serial.println("Tür benutzt!");
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2",jsonMessageBuffer);
  } 
  //sends a ping and the information if the sensor seems to work correctly every X minutes
  if (pu == 100) { //Erhöhen, wenn fertig getestet
    jsonDoc["msg-type"] = "ping";
    if (distance !=0){
      jsonDoc["value"] = "ok";
    } else {
      jsonDoc["value"] = "beeintraechtigt";
    }
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2/ping", jsonMessageBuffer);
    pu = 0;
  }
}

void bewegungsmelder()
{
  DynamicJsonDocument jsonDoc(512);
  char jsonMessageBuffer[100];
  
  pb = pb+1;
  
  jsonDoc["device-type"] = "HCSR501";
  jsonDoc["identifier"] = chipID + bewegungsID;
  
  // Read out the pirPin and store as val:
   val = digitalRead(pirPin);
  // If motion is detected, send JSON via mqtt
  if (val == HIGH) {
    jsonDoc["msg-type"] = "Aktivitaet";
    jsonDoc["value"] = "high";
    Serial.println("Motion detected!");
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2",jsonMessageBuffer);
  }
  if (pb == 100) { //Erhöhen, wenn fertig getestet
    jsonDoc["msg-type"] = "ping";
    jsonDoc["value"] = "ok";
    serializeJson(jsonDoc, jsonMessageBuffer);
    mqtt.publish("ttgo2/ping", jsonMessageBuffer);
    pb = 0;
  }
}

void loop() 
{
  ultraschall();
  bewegungsmelder();
  
  delay(200);
}
