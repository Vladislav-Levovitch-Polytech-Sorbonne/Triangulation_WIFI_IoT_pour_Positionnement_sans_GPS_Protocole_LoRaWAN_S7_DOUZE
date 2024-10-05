#include <ESP8266WiFi.h>

void setup() {
  delay(200);
  Serial.begin(9600);
  Serial.print("\nDeploiement du Mode STATION en cours ...\n");

  WiFi.mode(WIFI_STA);

}

void loop() 
{
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

    for (int8_t i = 0; i < Nb_Reseau; i++)
    {
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);

      Serial.print("+ ");
      Serial.print(i + 1);
      Serial.print(" / ");
      Serial.print(Nb_Reseau);
      Serial.print("\t+ RSSi = ");
      Serial.print(RSSI);
      Serial.print(" dB \t+ SSiD = ");
      Serial.println(ssid);

      yield(); //Securite pour verifier s il y a des interruptions
    }
  } 
}
