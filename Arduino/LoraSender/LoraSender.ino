/*********
 * Based on
  LoRa Sender
  pollux labs
  https://polluxlabs.net
*********/

//Bibliotheken für LoRa
#include <SPI.h>
#include <LoRa.h>

//Loramodul-Pins
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//Frequenzbereich Europa
#define BAND 866E6


//Counter für die gesendeten Datenpakete
int counter = 0;

void setup() {

  //Seriellen Monitor starten
  Serial.begin(115200);
  
  //Pins für LoRa setzen und Kommunikation starten
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa konnte nicht gestarten werden!");
    while (1);
  }
  Serial.println("LoRa OK!");
  delay(2000);
}

void loop() {
   
  Serial.print("Sende Paket: ");
  Serial.println(counter);

  //Datenpaket senden
  if(LoRa.beginPacket())
  {
    LoRa.print("Paket Nr. ");
    LoRa.print(counter);
    if(!LoRa.endPacket())
    {
      Serial.println("EndPacket Fehler!");
    }
    counter++;
  } else 
  {
    Serial.println("BeginPacket Fehler!");
  }


  delay(1000);
  if (counter == 1000){
    counter = 0;
  }
}
