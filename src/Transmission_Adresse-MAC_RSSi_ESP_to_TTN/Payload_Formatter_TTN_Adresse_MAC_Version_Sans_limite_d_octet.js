function Decoder(bytes, port) 
{
  let donnees_decompressees = {};
  let nb_WiFi = bytes[0];  // Nombre de reseaux en tete
  donnees_decompressees.nb_Reseaux = nb_WiFi;
  donnees_decompressees.nb_Appareils = [];

  for (let i = 0; i < nb_WiFi; i++) 
  {
    let start = 1 + i * 7;
    let MacHex = bytes.slice(start, start + 6).map(b => b.toString(16).padStart(2, '0')).join('-').toUpperCase();

    // Interpréter le RSSI en signed 8-bit
    let RssiHex = bytes[start + 6] > 127 ? bytes[start + 6] - 256 : bytes[start + 6];
    
    // Calculer la distance en fonction du RSSI
    let distance_estime;
    if (RssiHex > -56) 
    {
      distance_estime = parseFloat(Math.pow(10, -((RssiHex + 52) / 25.0)).toFixed(2)); //Pour avoir un nombre avec 2 decimales
    } 
    else 
    {
      distance_estime = parseFloat(Math.pow(10, -((RssiHex + 56) / 33.0)).toFixed(2)); //Pour avoir un nombre avec 2 decimales
    }

    donnees_decompressees.nb_Appareils.push
    ({
      Num   : `MAC_${i + 1}`,
      MAC   : MacHex,
      RSSi  : RssiHex,
      Distance_Estime: distance_estime
    });
  }

  return donnees_decompressees;
}

//Co coder avec Chat GPT

/*function decodeUplink(input) 
{
  //Fonction de conversion generee par ChatGPT : ASCii to Str
  let asciiString = String.fromCharCode.apply(null, input.bytes);

  let Caractere_1_to_2 = asciiString.substring(0, 2);
  let Caractere_3_to_4 = asciiString.substring(2, 4);
  let temperature = parseFloat(Caractere_1_to_2 + "." + Caractere_3_to_4);

  let Caractere_5_to_6 = asciiString.substring(4, 6);
  let Caractere_7_to_8 = asciiString.substring(6, 8);
  let humidite = parseFloat(Caractere_5_to_6 + "." + Caractere_7_to_8);

  return {
    data: {
      A_Temperature : temperature,
      B_Humidite : humidite
    },
    warnings: [],
    errors: []
  };
}*/

/*function Decoder(bytes, port)
{
  let decoded = {};
  let nbMacs = parseInt(bytes[0].toString(16), 16); // Nombre d'adresses MAC au début
  decoded.nbMacs = nbMacs;
  decoded.devices = [];

  for (let i = 0; i < nbMacs; i++) 
  {
    let start = i * 7 + 1;  // Chaque entrée = 6 octets pour MAC + 1 octet pour RSSI, commence après l'octet de quantité
    let macHex = bytes.slice(start, start + 6).map(b => b.toString(16).padStart(2, '0')).join('-');
    let rssi = bytes[start + 6] - 256;  // Convertir en dBm (octet signé)
    
    decoded.devices.push({
      mac: macHex.toUpperCase(),
      rssi: rssi
    });
  }

  return decoded;
}*/