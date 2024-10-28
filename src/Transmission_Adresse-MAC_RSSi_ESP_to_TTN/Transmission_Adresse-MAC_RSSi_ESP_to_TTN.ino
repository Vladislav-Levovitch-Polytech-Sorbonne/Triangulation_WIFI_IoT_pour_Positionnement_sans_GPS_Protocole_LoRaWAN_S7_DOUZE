#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial Serial_to_LoRa (D5, D6); //D5 : GPIO14 Rx to Lora D6 : GPIO12 Tx to Lora 
//GPIO3 to RX, GPIO1 to TX reserved to ESP8266 to USB convert

void Serial_to_LoRa_function(String commande, String texte_affiche, int delayTime) 
{
  Serial_to_LoRa.write(commande.c_str());
  Serial.print(Serial_to_LoRa.readString());
  
  delay(delayTime);
}

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
  Serial_to_LoRa_function("AT+DR=EU868", " \"AT+DR=EU868\"", 5000);
  Serial_to_LoRa_function("AT+MODE=LWOTAA", " \"AT+MODE=LWOTAA\"", 5000);
  Serial_to_LoRa_function("AT+JOIN", " \"AT+JOIN\"", 10000);
  Serial_to_LoRa_function("AT", " \"AT\"", 2000); 
  Serial.print("\nDeploiement du Mode STATION pour la recolte des adresses MAC et RSSI environnantes en cours ...\n");
  WiFi.mode(WIFI_STA);
  delay(2000);
}

void loop() 
{
  String Message_Compresse;

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    if (command.equalsIgnoreCase("STOP"))
    {
      Serial.print("Arret de l'affichage des RSSI et infos reseaux\tPour reprendre envoyer \"ON\"\n");
      while (true)
      {
        if (Serial.available() > 0)
        {
          String resumeCommand = Serial.readStringUntil('\n');
          if (resumeCommand.equalsIgnoreCase("ON"))
          {
            Serial.println("Demarrage de l'affichage\tPour arreter envoyer \"STOP\"");
            break;
          }
        }
      }
    }
  }

  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t *BSSID;
  int32_t channel;
  bool isHidden;

  int Nb_Reseau = WiFi.scanNetworks(false, true);
  int Nb_Reseau_Forced = 7;

  if (Nb_Reseau == 0) 
  {
    Message_Compresse = "0";
    Serial.print("Vous etes a la campagne : pas de reseau detecte ...\n");
  } 
  else if (Nb_Reseau > 0) 
  {
    Serial.print("Nombre de reseau trouve : ");
    Serial.println(Nb_Reseau);
    Message_Compresse = String(Nb_Reseau_Forced, HEX);

    for (int i = 0; i < Nb_Reseau_Forced; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);

      Serial.print("+ ");
      Serial.print(i + 1);
      Serial.print(" / ");
      Serial.print(Nb_Reseau);

      Serial.print("\t+ MAC = ");
      for (int ii = 0; ii < 6; ii++)
      {
        if (BSSID[ii] < 16)
        {
          Serial.print("0");
          Message_Compresse += "0";
        }
        Serial.print(BSSID[ii], HEX);
        Message_Compresse += String(BSSID[ii], HEX);

        if (ii < 5)
        {
          Serial.print("-");
        }
      }
      Serial.print("\t+ RSSI = ");
      Serial.print(RSSI);
      Serial.print(" dB => Distance_Estime = ");

      if (RSSI > -56)
      {
        Serial.print(pow(10, -((RSSI + 52) / 25.0)));
      }
      else
      {
        Serial.print(pow(10, -((RSSI + 56) / 33.0)));
      }

      Serial.print("m\t\t+ SSID = ");
      Serial.println(ssid);

      // Convertir RSSI en chaine hexadecimale et ajouter au Message_Compresse
      Message_Compresse += String(abs(RSSI), HEX);

      yield();
    }
  }

  Serial.print("\nDebut de transmission :\n");
  if (Serial_to_LoRa.readString())
  {
    // Envoi direct de la commande AT+MSGHEX sans fonction
    String commande = "AT+MSGHEX=\"" + Message_Compresse + "\"";
    Serial_to_LoRa.write(commande.c_str());
    Serial.print(Serial_to_LoRa.readString());
  }
  else
  {
    Serial.print("Attente de reponse - Cablage a verifier\n");
  }

  int nb_point = 10;
  int delay_tot = 10000; 
  for (int i = 0; i < nb_point; i++)
  {
    Serial.printf(" .");
    if (i == nb_point - 1)
    {
      Serial.print("\n");
    }
    delay(delay_tot / nb_point);
  }
}
