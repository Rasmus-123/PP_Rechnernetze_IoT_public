/*********
  LoRa Receiver
  pollux labs
  https://polluxlabs.net
*********/

//TODO: Falls Fehler beim Testen: Klammern in der Loop überprüfen!

#include <LoRa.h>
#include <MQTTPubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// RasPi-WLAN
const char* ssid = "PiLAN";
const char* pass = "raspberrypi42";

uint32_t chipID;
uint32_t sensorID = 7054;

WiFiClient client;
MQTTPubSubClient mqtt;


//Pins, an denen das LoRa-Modul am ESP32 angeschlossen ist
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//Frequenzbereich für Europa
#define BAND 866E6

//Counter für die gesendeten Datenpakete
int counter = 0;
int p = 100;
int rssi;


//Variable für die empfangenen Daten
String LoRaData;

void setup() {
    //Seriellen Monitor starten
    Serial.begin(115200);
    chipID = (ESP.getEfuseMac() << 40) >> 40;
    WiFi.setHostname("LoRaReceiver");
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

    // initialize mqtt client
    mqtt.begin(client);

    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect("LoRaReceiver", "public", "public")) 
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");
    //Pins für LoRa definieren und Kommunikation starten
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);
  
    if (!LoRa.begin(BAND)) {
      Serial.println("LoRa konnte nicht gestarten werden!");
      while (1);
    }
    Serial.println("LoRa  OK!");
}

void loop() {
  //Datenpaket parsen
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    p = p + 1;
    
    DynamicJsonDocument jsonDoc(512);
    char jsonMessageBuffer[100];
    
    //Namen anpassen
    jsonDoc["device-type"] = "LoraReceiver";
    // Unterschiedlich je Sensor vermutlich am Sinnvollsten. Also noch was anhängen oder so.
    jsonDoc["identifier"] = chipID + sensorID;
    mqtt.update();
    //Serial.print("Paket empfangen ");

    //Paket auslesen
    while (LoRa.available()) 
    {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //RSSI ausgeben
    
    rssi = LoRa.packetRssi();
    Serial.print(" mit RSSI ");
    Serial.println(rssi);
    
    //Problem: Einrichtung. Welche Werte passen muss in der tatsächlichen Umgebung geprüft werden.
    //Wann befindet sich jemand im Raum?
    if(1)
    {
      //hier wieder aktualisieren!
      jsonDoc["msg-type"] = "lora";
      jsonDoc["value"]= rssi;
      Serial.println("Aktivität!");
      serializeJson(jsonDoc, jsonMessageBuffer);
      mqtt.publish("ttgo2",jsonMessageBuffer);
    }

    
    if (p >= 100) 
    {
      Serial.println("Ping!");
      jsonDoc["msg-type"] = "ping";
      //Anpassen: Wann ist Funktion beeinträchtigt?
      if (rssi >= -100)
      {
        jsonDoc["value"] = "ok";
      } 
      else 
      {
        jsonDoc["value"] = "beeintraechtigt";
      }
      serializeJson(jsonDoc, jsonMessageBuffer);
      mqtt.publish("ttgo2/ping", jsonMessageBuffer);
      p = 0;
    }
  }
}
