#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial Serial_to_LoRa (D5, D6); //D5 : GPIO14 Rx to Lora D6 : GPIO12 Tx to Lora 
//GPIO3 to RX, GPIO1 to TX reserved to ESP8266 to USB convert

//Envoi commande via le port serie de LoRa to TTN et affaichage de commande par liasion serie de ESP to PC
void Serial_to_LoRa_function(String commande, String texte_affiche, int delayTime) 
{
  //Serial.print("\nSerial_to_LoRa" + texte_affiche);  //Utile au deboguage
  Serial_to_LoRa.write(commande.c_str());
  Serial.print(Serial_to_LoRa.readString());
  
  delay(delayTime);
}

//Configuration du Module LoRa E5 pour envoie sur le reseau TTN
void setup()
{
  delay (3000);
  Serial.println("\nDebut du programme :");
  Serial.begin(9600); //Monitor
  Serial_to_LoRa.begin(9600); //LoRa  
  delay (2000);

  Serial.println();
  Serial_to_LoRa_function("AT", " \"AT\"", 2000); 

  //Configuration des parametres de connection
  Serial_to_LoRa_function("AT+ID=DevEUI,70B3D57ED006AE59", " \"AT+ID=DevEUI\"", 5000);
  Serial_to_LoRa_function("AT+ID=AppEUI,A0A0A0A0A0A0A0A0", " \"AT+ID=AppEUI\"", 5000);
  Serial_to_LoRa_function("AT+KEY=APPKEY,B3C21FF5B9ECD87A0BB702B2C3D87459", " \"AT+KEY=APPKEY\"", 5000);
  Serial.println();
  //Commandes Annexes
  Serial_to_LoRa_function("AT+DR=EU868", " \"AT+DR=EU868\"", 5000);
  Serial_to_LoRa_function("AT+MODE=LWOTAA", " \"AT+MODE=LWOTAA\"", 5000);
  Serial_to_LoRa_function("AT+JOIN", " \"AT+JOIN\"", 10000);
  Serial_to_LoRa_function("AT", " \"AT\"", 2000); 
  //Demarrage de la recolte des donnees
  Serial.print("\nDeploiement du Mode STATION pour la recolte des adresses MAC et RSSi environnantes en cours ...\n");
  WiFi.mode(WIFI_STA);
  delay(2000);
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

  for (int i = 0; i < 5; i++) 
  {
    Serial.printf(" .");
    if (i == 4)
    {
      Serial.print("\n\n");
    }
    delay(500);
  }

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

  Serial.print("\nDebut de transmission :\n");
  if (Serial_to_LoRa.readString())
  {
    //AT+MSGHEX="01 00246CBC90E0 A5"
    Serial.print("Envoi des donnees MAC RRSi sur le Reseau TTN\n");
    Serial_to_LoRa.write("AT+MSGHEX=\"01 00246CBC90E0 A5\"");
    //Serial_to_LoRa.write(("AT+MSGHEX=" + String(ii+20)).c_str());
    Serial.print(Serial_to_LoRa.readString());
  }
  else
  {
    Serial.print("Attente de reponse - Cablage a verifier\n");
  }

  //Pause visuelle avec des petits points pour savoir que le systeme n est pas bloque
  int nb_point = 10;
  int delay_tot = 10000; //ms
  for (int i = 0; i < nb_point; i++)
  {
    Serial.printf(" .");
    if (i == nb_point-1)
    {
      Serial.print("\n");
    }
    delay(delay_tot/nb_point);
  }
}
