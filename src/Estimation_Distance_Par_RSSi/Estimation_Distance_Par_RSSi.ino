#include <ESP8266WiFi.h>

void setup() 
{
  delay(200);
  Serial.begin(9600);
  Serial.print("\nDeploiement du Mode STATION en cours ...\n");

  WiFi.mode(WIFI_STA);
}

void loop() 
{
  //Communication Serie pour log et affichage
  if (Serial.available() > 0) //Verification asynchrone de la disponibilite d une intruction
  {
    String command = Serial.readStringUntil('\n'); //Lecture instruction
    if (command.equalsIgnoreCase("STOP")) //IgnoreCase permet de pouvoir ecrire la consigne avec des minuscules inclues en cas d oublie de maj
    {
      Serial.print("Arret de l affichage des RSSi et infos réseaux \tPour reprendre envoyer \"ON\"\n");
      while (true) 
      {
        if (Serial.available() > 0) //Verification reprise d affichage
        {
          String resumeCommand = Serial.readStringUntil('\n');
          if (resumeCommand.equalsIgnoreCase("ON")) 
          {
            Serial.println("Demarrage de l affichage\tPour arreter envoyer\"STOP\"");
            break;
          }
        }
      }
    }
  }

  //Variables pour le getNetworkInfo
  String ssid;  //WiFi.SSID()
  uint8_t encryptionType; 
  int32_t RSSI; //WiFi.RSSI()
  uint8_t *BSSID;
  int32_t channel;
  bool isHidden;
  int Nb_Reseau = WiFi.scanNetworks(false, true);

  if (Nb_Reseau == 0) 
  {
    Serial.print("Vous etes a la campagne : pas de reseau detecte ...\3n");
  } 
  
  else if (Nb_Reseau > 0) 
  {
    Serial.print("Nombre de reseau trouve : ");
    Serial.println(Nb_Reseau);

    //Affichage des donnees de chaque reseau
    for (int i = 0; i < Nb_Reseau; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);

      Serial.print("+ ");
      Serial.print(i + 1);
      Serial.print(" / ");
      Serial.print(Nb_Reseau);

      Serial.print("\t+ MAC = ");
      for (int i = 0; i < 6; i++)
      {
        if (BSSID[i] < 16) 
        {
          Serial.print("0"); //Cas ou un 0 est en Hex de poid fort sinon printf et PSTR
        }
        Serial.print(BSSID[i],HEX);

        if (i < 5)
        {
          Serial.print("-");
        }
      }
      Serial.print("\t+ RSSi = ");
      Serial.print(RSSI);
      Serial.print(" dB => Distance_Estime = ");

      if (RSSI > -56) 
      {
        Serial.print(pow(10, -((RSSI + 52) / 25.0))); //Environnement degage
      } 

      else 
      {
        Serial.print(pow(10, -((RSSI + 56) / 33.0))); //Environnement a faible contrainte
      } 

      Serial.print("m\t\t+ SSiD = ");
      Serial.println(ssid);

      yield(); //Securite pour verifier s il y a des interruptions
    }
  } 
}
