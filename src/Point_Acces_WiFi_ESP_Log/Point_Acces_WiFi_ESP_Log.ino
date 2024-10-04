// Reutilisation d un example ARDUINO pour la mise en place d un AP ( Acces Point )

#include <ESP8266WiFi.h>

#ifndef PASSID_Param
#define PASSID "ESP8266_acces_point_deplaceable" //Remplacer deplaceable par numero de salle si sur site
#endif

const char *ssid = PASSID;

void setup() 
{
  delay(200);
  Serial.begin(9600); //Baudrate Standard pour echange simple de log dans le serial monitor
  Serial.println("\nDeploiement Point Acces en cours ...");
  
  WiFi.softAP(ssid);
}

void loop() 
{
  if (Serial.available() > 0) //Verification asynchrone de la disponibilite d une intruction
  {
    String command = Serial.readStringUntil('\n'); //Lecture instruction
    if (command.equalsIgnoreCase("STOP")) 
    {
      Serial.print("Arret de l affichage de l adresse MAC\tPour reprendre envoyer \"MAC\"\n");
      while (true) 
      {
        if (Serial.available() > 0) //Verification reprise d affichage
        {
          String resumeCommand = Serial.readStringUntil('\n');
          if (resumeCommand.equalsIgnoreCase("MAC")) 
          {
            Serial.println("Demarrage de l affichage\tPour arreter envoyer\"STOP\"");
            break;
          }
        }
      }
    }
  }

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