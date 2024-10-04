// Reutilisation d un example ARDUINO pour la mise en place d un AP ( Acces Point )

#include <ESP8266WiFi.h>

#ifndef PASSID_Param
#define PASSID "ESP8266_acces_point_deplaceable"
#endif

const char *ssid = PASSID;

void setup() 
{
  delay(200);
  Serial.begin(9600); //Baudrate Standard pour echange simple de log dans le serial monitor
  Serial.println("Deploiement Point Acces en cours ...");
  
  WiFi.softAP(ssid);
}

void loop() 
{
  String Addresse_MAC = WiFi.macAddress();  //Recuperation adresse MAC
  Serial.print("PA - Adresse MAC : " + Addresse_MAC + "\t-\tSSID : " + ssid); //Rappel adresse MAC
  
  
  for (int i = 0; i < 3; i++) 
  {
    Serial.printf(" .");
    if (i == 2)
    {
      Serial.println();
    }
    delay(1000);
  }
}