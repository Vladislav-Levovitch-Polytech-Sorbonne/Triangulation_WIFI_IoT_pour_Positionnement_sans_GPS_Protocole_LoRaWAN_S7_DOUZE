#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial Serial_to_LoRa (D5, D6); // D5 : GPIO14 Rx to Lora, D6 : GPIO12 Tx to Lora

// Fonction pour envoyer des commandes LoRa et afficher le retour
void Serial_to_LoRa_function(String commande, String texte_affiche, int delayTime) 
{
  Serial_to_LoRa.write(commande.c_str());
  Serial.print(Serial_to_LoRa.readString());
  delay(delayTime);
}

void setup()
{
  delay(3000);
  Serial.println("\nDebut du programme :");
  Serial.begin(9600); // Monitor
  Serial_to_LoRa.begin(9600); // LoRa
  delay(2000);

  Serial.println();
  Serial_to_LoRa_function("AT", " \"AT\"", 2000);

  // Configuration des parametres de connexion
  Serial_to_LoRa_function("AT+ID=DevEUI,70B3D57ED006AE59", " \"AT+ID=DevEUI\"", 5000);
  Serial_to_LoRa_function("AT+ID=AppEUI,A0A0A0A0A0A0A0A0", " \"AT+ID=AppEUI\"", 5000);
  Serial_to_LoRa_function("AT+KEY=APPKEY,B3C21FF5B9ECD87A0BB702B2C3D87459", " \"AT+KEY=APPKEY\"", 5000);
  
  Serial_to_LoRa_function("AT+DR=EU868", " \"AT+DR=EU868\"", 5000);
  Serial_to_LoRa_function("AT+MODE=LWOTAA", " \"AT+MODE=LWOTAA\"", 5000);
  Serial_to_LoRa_function("AT+JOIN", " \"AT+JOIN\"", 10000);

  Serial.print("\nDeploiement du Mode STATION pour la recolte des adresses MAC et RSSi environnantes en cours ...\n");
  WiFi.mode(WIFI_STA);
  delay(2000);
}

void loop() 
{
  String Message_Compresse;
  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t *BSSID;
  int32_t channel;
  bool isHidden;

  int Nb_Reseau = WiFi.scanNetworks(false, true);
  int Nb_Chaine = (Nb_Reseau + 6) / 7; // Calcule le nombre de chaines (paquets de 7 max)

  if (Nb_Reseau == 0) 
  {
    Serial.print("Vous etes a la campagne : pas de reseau detecte ...\n");
    return;
  }

  Serial.print("Nombre de reseau trouve : ");
  Serial.println(Nb_Reseau);

  for (int chaine_index = 0; chaine_index < Nb_Chaine; chaine_index++) 
  {
    Message_Compresse = ""; // Reinitialise pour chaque chaine

    //Premier octet : 4 bits de poids fort pour le numero de l envoi, 4 bits de poids faible pour le nombre total d envois
    uint8_t header1 = (chaine_index + 1) << 4 | Nb_Chaine;
    Message_Compresse += (header1 < 16 ? "0" : "") + String(header1, HEX);

    //Deuxieme octet : Nombre total de reseaux detectes
    Message_Compresse += (Nb_Reseau < 16 ? "0" : "") + String(Nb_Reseau, HEX);

    //Remplit la chaine avec jusqu a 7 reseaux
    for (int i = chaine_index * 7; i < (chaine_index + 1) * 7 && i < Nb_Reseau; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);

      // Affichage detaille pour chaque reseau
      Serial.print("+ ");
      Serial.print(i + 1);
      Serial.print(" / ");
      Serial.print(Nb_Reseau);
      Serial.print("\t+ MAC = ");
      for (int j = 0; j < 6; j++)
      {
        if (BSSID[j] < 16) 
        {
          Serial.print("0");
        }
        Serial.print(String(BSSID[j], HEX));
        if (j < 5) Serial.print("-");
      }
      Serial.print("\t+ RSSI = ");
      Serial.print(RSSI);
      Serial.print(" dB => Distance_Estime = ");
      
      //Calcul de la distance estimee en fonction du RSSI 
      float distance_estime = (RSSI > -56) 
                                ? pow(10, -((RSSI + 52) / 25.0)) /*Equation pour un milieu proche et degage*/
                                : pow(10, -((RSSI + 56) / 33.0));/*Equation pour a plus d un metre et avec legers obstacles*/
      Serial.print(distance_estime, 2);
      Serial.print("m");
      Serial.print("\t\t+ SSID = ");
      Serial.println(ssid);

      //Ajoute l adresse MAC au Message_Compresse
      for (int j = 0; j < 6; j++)
      {
        if (BSSID[j] < 16)
        {
          Message_Compresse += "0";
        }
        Message_Compresse += String(BSSID[j], HEX);
      }

      //Ajoute le RSSI converti en positif pour simplifier l hexadecimal
      int rssiHex = RSSI < 0 ? 256 + RSSI : RSSI;
      if (rssiHex < 16)
      {
        Message_Compresse += "0";
      }
      Message_Compresse += String(rssiHex, HEX);
    }

    //Envoi direct du paquet au module LoRa
    Serial.print("\nDebut de transmission de la chaine ");
    Serial.print(chaine_index + 1);
    Serial.print(" sur ");
    Serial.print(Nb_Chaine);
    Serial.print(" :\n");

    String commande = "AT+MSGHEX=\"" + Message_Compresse + "\"";
    Serial_to_LoRa.write(commande.c_str());
    Serial.println(commande);
    Serial.print(Serial_to_LoRa.readString());

    //Pause entre chaque envoi
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
  Serial.print("Fin de la transmission de toutes les chaines.\n");
}