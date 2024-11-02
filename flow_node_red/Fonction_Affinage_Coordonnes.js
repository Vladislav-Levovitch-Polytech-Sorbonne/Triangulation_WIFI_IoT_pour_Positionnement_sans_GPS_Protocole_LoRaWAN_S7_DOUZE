// Recuperation de la liste de la base de donnees
let liste_BDD = global.get("base_de_donnees") || [];

// Initialisation ou recuperation des variables globales pour le calcul
let numerateur_pondere_x = global.get("numerateur_pondere_x") || 0;
let numerateur_pondere_y = global.get("numerateur_pondere_y") || 0;
let denominateur_pondere_x = global.get("denominateur_pondere_x") || 0;
let denominateur_pondere_y = global.get("denominateur_pondere_y") || 0;

// Parcours de chaque appareil dans Donnees_Appareils
msg.payload.Donnees_Appareils.forEach(appareil => 
{
    // Convertir l'adresse MAC reçue en minuscule pour la comparaison
    let macRecu = appareil.MAC.toLowerCase();

    // Chercher l'appareil dans liste_BDD en comparant les MAC en minuscules
    let match = liste_BDD.find(item => item.MAC.toLowerCase() === macRecu);

    if (match && appareil.Distance_Estime) 
    {
        // Calcul du coefficient de ponderation base sur Distance_Estime
        let coefficient_de_ponderation = 2 / Math.pow(appareil.Distance_Estime, 0.65);

        // Incrementer les numerateurs et denominateurs ponderes
        numerateur_pondere_x += match.CurrentLatitude * coefficient_de_ponderation;
        denominateur_pondere_x += coefficient_de_ponderation;

        numerateur_pondere_y += match.CurrentLongitude * coefficient_de_ponderation;
        denominateur_pondere_y += coefficient_de_ponderation;
    }
});

// Calcul des coordonnees ponderees apres chaque paquet
let coordonnee_ponderee_x = denominateur_pondere_x !== 0 ? numerateur_pondere_x / denominateur_pondere_x : null;
let coordonnee_ponderee_y = denominateur_pondere_y !== 0 ? numerateur_pondere_y / denominateur_pondere_y : null;

// Ajout des coordonnees calculees au message de sortie
msg.payload = 
{
    coordonnee_ponderee_x: coordonnee_ponderee_x,
    coordonnee_ponderee_y: coordonnee_ponderee_y
};

// Mettre a jour les variables globales pour stocker l'etat courant
global.set("numerateur_pondere_x", numerateur_pondere_x);
global.set("numerateur_pondere_y", numerateur_pondere_y);
global.set("denominateur_pondere_x", denominateur_pondere_x);
global.set("denominateur_pondere_y", denominateur_pondere_y);

// Si tous les paquets ont ete reçus, reinitialiser les variables globales
if (msg.payload.numero_envoi === msg.payload.total_envoi) 
{
    global.set("numerateur_pondere_x", 0);
    global.set("numerateur_pondere_y", 0);
    global.set("denominateur_pondere_x", 0);
    global.set("denominateur_pondere_y", 0);
}

return msg;

//Co-Commente par ChatGPT