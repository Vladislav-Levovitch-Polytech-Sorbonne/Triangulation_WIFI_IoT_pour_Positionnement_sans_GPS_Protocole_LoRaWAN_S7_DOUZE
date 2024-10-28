function Decoder(bytes, port) 
{
  let donnees_decompressees = {};
  
  //2 premiers octet d information
  let envoi_total = bytes[0];
  let numero_envoi = (envoi_total >> 4) & 0x0F;    //4 bits de poids fort : le 1er quartet contient le numero de l envoi courent
  let total_envoi = envoi_total & 0x0F;            //4 bits de poids faible : le 2eme quartet le nb d envoi total programme

  let nb_envois_restants = bytes[0];
  let nb_total_envois = bytes[1];
  let nb_WiFi = bytes[1];
  
  donnees_decompressees.numero_envoi = numero_envoi;
  donnees_decompressees.total_envoi = total_envoi;
  donnees_decompressees.nb_Reseaux = nb_WiFi;
  donnees_decompressees.Donnees_Appareils = [];
  
  let nb_de_donnees_a_traiter = (nb_total_envois-nb_envois_restants)>0 ? 7 : nb_WiFi%7;

  for (let i = 0; i < nb_de_donnees_a_traiter; i++) 
  {
    let start = 2 + i * 7; //Decalage du au 2 premiers octet d information
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

    donnees_decompressees.Donnees_Appareils.push
    ({
      Num   : `MAC_${(i + 1 ) + ((numero_envoi - 1 ) *(7))}`,
      MAC   : MacHex,
      RSSi  : RssiHex,
      Distance_Estime: distance_estime
    });
  }

  return donnees_decompressees;
}

//Co coder avec Chat GPT