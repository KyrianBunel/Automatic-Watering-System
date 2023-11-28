/*

...............................BIBLIOTHEQUES...............................

*/
#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WebServer.h>
#include <ArduinoOTA.h>
#include "DHT.h"
#include <HTTPClient.h>
#include "time.h"

// Google script ID
String GOOGLE_SCRIPT_ID = "YOUR_GOOGLESHEET_ID";  // change Gscript ID

// Nombre de vanne connectées
#define NbVannes 3

#define DHTPIN 27
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
struct tm timeinfo;

/*

...............................VARIABLES...............................

*/
String version_logiciel = "5.1.1_en";    // Software version

const char *ssid = "****";            // SSID WiFI
const char *password = "****";        // Mot de passe WiFi

WebServer server(80);  // Server sur le port 80 (page web)

const char *ntpServer = "pool.ntp.org";  // adresse du serveur NTP
const long gmtOffset_sec = 3600 * 2;     // fuseau horraire (paris)
const int daylightOffset_sec = 3600 * 0;

int SeuilHumidite = 10;  // humididé dans le sol
float SeuilTemperatureMAX = 25.00;
float SeuilTemperatureMIN = 15.00;
int SeuilHumidity = 40.00;  // humidité dans l'air

int TimerAUTO = 0;       // Mesurer le temps d'arrosage en mode AUTO
int delayAUTO = 600000;  // temps d'arrosage en mode AUTO (en ms)
int ModeAuto = 1;        // activation ou non de l'arrosage en mode auto
int JourArrosage;

int bpVanne1 = 0;  // Bouton vanne 1 du mode MANU
int bpVanne2 = 0;  // Bouton vanne 2 du mode MANU
int bpVanne3 = 0;  // Bouton vanne 3 du mode MANU

int MoistureSensor1;  // Capteur d'humidité dans le sol 1
int MoistureSensor2;  // Capteur d'humidité dans le sol 2
int MoistureSensor3;  // Capteur d'humidité dans le sol 3

int pluie;        // Valeur du capteur de pluie
String StrPluie;  // Valeur du capteur de pluie (True/False)

float humidity;     // Valeur humidité
float temperature;  // Valeur température

int luminosite;  // Valeur du capteur de luminosité

int Hour;   // heure actuelle
int Min;    // minute actuelle
int Sec;    // seconde actuelle
int Day;    // jour actuel
int Month;  // mois actuel
int Year;   // année actuelle

int en;

#define vanne1 12        // Vanne 1 sur GPIO12 de l'esp
#define vanne2 32        // Vanne 2 sur GPIO32 de l'esp
#define vanne3 25        // Vanne 3 sur GPIO25 de l'esp
#define CapteurPluie 36  // Capteur de pluie sur GPIO36 de l'esp

char StrHour[40];                                                          // formattage de l'heure en chaîne de caractère
char StrDate[40];                                                          // formattage de la date en chaîne de caractère
char HtmlON[50] = "<div class='rectangleON'>ON</div>";                     // afficher un carré vert marqué ON
char HtmlOFF[50] = "<div class='rectangleOFF'>OFF</div>";                  // afficher un carré rouge marqué OFF
char HtmlDISABLED[50] = "<div class='rectangleDISABLED'>DÉSACTIVÉ</div>";  // afficher un carré violet marqué DISABLED
char HtmlZone1[50];                                                        // afficher état de la zone 1 dans le dashboard
char HtmlZone2[50];                                                        // afficher état de la zone 2 dans le dashboard
char HtmlZone3[50];                                                        // afficher état de la zone 3 dans le dashboard

float usagePercentage;  // Utilisation de la mémoire en %

String mode = "MANU";  // Initialisation de la variable "mode" (valeur par défaut = MANU)

String HTMLtabl;  // Initialisation de la viriable tabl (calendrier)

/* Date de début */
String StrDateDebut;   // enregistrer la date de début rentré dans le formulaire
String StrStartDay;    // Jour de départ (string)
String StrStartMonth;  // Mois de départ (string)
String StrStartYear;   // Année de départ (string)
int StartDay;          // Jour de départ (int)
int StartMonth;        // Mois de départ (int)
int StartYear;         // Année de départ (int)

/* Date de fin */
String StrDateFin;   // enregistrer la date de début rentré dans le formulaire
String StrEndDay;    // Jour de fin (string)
String StrEndMonth;  // Mois de fin (string)
String StrEndYear;   // Année de fin (string)
int EndDay;          // Jour de fin (int)
int EndMonth;        // Mois de fin (int)
int EndYear;         // Année de fin (int)

String StrDuree;  // Dureé du programme (string)
int duree;        // Dureé du programme (int)

String StrRecurrence;  // Récurence (string)
int recurrence;        // Récurence (int)

/* Zone 1 */
String StrZone1;  // Activation de la zone 1 ou non
String StrDebut1;
String StrHeureDebut1;   // Heure de début (string)
String StrMinDebut1;     // Minute de début (string)
char IntervalZone1[40];  // interval d'arrosage (chaîne de carractère)
int HeureDebut1;         // Heure de début (int)
int MinDebut1;           // Minute de début (int)
int EndMin1;             // Minute de fin (int)
int EndHour1;            // Heure de fin (int)

/* Zone 2 */
String StrZone2;  // Activation de la zone 2 ou non
String StrDebut2;
String StrHeureDebut2;   // Heure de début (string)
String StrMinDebut2;     // Minute de début (string)
char IntervalZone2[40];  // interval d'arrosage (chaîne de carractère)
int HeureDebut2;         // Heure de début (int)
int MinDebut2;           // Minute de début (int)
int EndMin2;             // Minute de fin (int)
int EndHour2;            // Heure de fin (int)

/* Zone 3 */
String StrZone3;  // Activation de la zone 3 ou non
String StrDebut3;
String StrHeureDebut3;   // Heure de début (string)
String StrMinDebut3;     // Minute de début (string)
char IntervalZone3[40];  // interval d'arrosage (chaîne de carractère)
int HeureDebut3;         // Heure de début (int)
int MinDebut3;           // Minute de début (int)
int EndMin3;             // Minute de fin (int)
int EndHour3;            // Heure de fin (int)

/*

...............................FONCTIONS...............................

*/
/* Calculer le premier jour du mois (Algorithme de Zeller) */
int calculerPremierJour(int annee, int mois) {
  int a = (14 - mois) / 12;
  int y = annee - a;
  int m = mois + 12 * a - 2;
  int jour = (1 + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;
  jour--;
  return jour;  // Retourne le jour de la semaine
}

/* Calculer le nombre de jours en fonction du mois */
int getNumberOfDays(int year, int month) {
  switch (month) {
    case 1:   // Janvier
    case 3:   // Mars
    case 5:   // Mai
    case 7:   // Juillet
    case 8:   // Août
    case 10:  // Octobre
    case 12:  // Décembre
      return 31;
    case 4:   // Avril
    case 6:   // Juin
    case 9:   // Septembre
    case 11:  // Novembre
      return 30;
    case 2:  // Février
      if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 29;  // Année bissextile
      } else {
        return 28;
      }
    default:
      return 0;  // Mois invalide
  }
}

void sendData() {
  String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "date=" + Day + "/" + Month + "/" + Year + "_" + String(Hour) + ":" + String(Min) + ":" + String(Sec) + "&temperature=" + String(temperature) + "&humidite=" + String(humidity) + "&luminosite=" + String(luminosite) + "&moistureSensor1=" + String(MoistureSensor1) + "&moistureSensor2=" + String(MoistureSensor2) + "&moistureSensor3=" + String(MoistureSensor3);
  HTTPClient http;
  http.begin(urlFinal.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  http.end();
}

/* Gestion du bouton "MANU" */
void handleMANU() {
  mode = "MANU";
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/* Gestion du bouton "AUTO" */
void handleAUTO() {
  mode = "AUTO";
  bpVanne1 = 0;
  bpVanne2 = 0;
  bpVanne3 = 0;
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/* Gestion du bouton "PROG" */
void handlePROG() {
  mode = "PROG";
  bpVanne1 = 0;
  bpVanne2 = 0;
  bpVanne3 = 0;
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/* Gestion du bouton Vanne 1 du mode MANU (activation/déactivation de l'arrosage) */
void handleVanne1() {
  if (bpVanne1 == 0 && mode == "MANU") {
    bpVanne1 = 1;
  } else
    bpVanne1 = 0;
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/* Gestion du bouton Vanne 2 du mode MANU (activation/déactivation de l'arrosage) */
void handleVanne2() {
  if (bpVanne2 == 0) {
    bpVanne2 = 1;
  } else
    bpVanne2 = 0;
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/* Gestion du bouton Vanne 3 du mode MANU (activation/déactivation de l'arrosage)*/
void handleVanne3() {
  if (bpVanne3 == 0) {
    bpVanne3 = 1;
  } else
    bpVanne3 = 0;
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/*  Gestion du formulaire du mode PROG  */
void handleSubmit() {
  // Récupération des valeurs du formulaire
  StrDuree = server.arg("duree");
  StrDateDebut = server.arg("dateDebut");
  StrDateFin = server.arg("dateFin");
  StrRecurrence = server.arg("recurrence");
  StrZone1 = server.arg("zone1");
  StrZone2 = server.arg("zone2");
  StrZone3 = server.arg("zone3");
  StrDebut1 = server.arg("heureDebut1");
  StrDebut2 = server.arg("heureDebut2");
  StrDebut3 = server.arg("heureDebut3");



  /* Date de début (conversion str -> int) */
  StrStartDay = StrDateDebut.substring(8, 10);
  StrStartMonth = StrDateDebut.substring(5, 7);
  StrStartYear = StrDateDebut.substring(0, 4);
  StartDay = StrStartDay.toInt();
  StartMonth = StrStartMonth.toInt();
  StartYear = StrStartYear.toInt();

  /* Date de fin (conversion str -> int) */
  StrEndDay = StrDateFin.substring(8, 10);
  StrEndMonth = StrDateFin.substring(5, 7);
  StrEndYear = StrDateFin.substring(0, 4);
  EndDay = StrEndDay.toInt();
  EndMonth = StrEndMonth.toInt();
  EndYear = StrEndYear.toInt();

  /* Durée du programme (conversion str -> int)*/
  duree = StrDuree.toInt();

  /* Récurrence (conversion str -> int) */
  recurrence = StrRecurrence.toInt();

  /* Zone 1 (conversion str -> int) */
  StrHeureDebut1 = StrDebut1.substring(0, 2);
  StrMinDebut1 = StrDebut1.substring(3, 5);
  HeureDebut1 = StrHeureDebut1.toInt();
  MinDebut1 = StrMinDebut1.toInt();

  /* Zone 2 (conversion str -> int) */
  StrHeureDebut2 = StrDebut2.substring(0, 2);
  StrMinDebut2 = StrDebut2.substring(3, 5);
  HeureDebut2 = StrHeureDebut2.toInt();
  MinDebut2 = StrMinDebut2.toInt();

  /* Zone 3 (conversion str -> int) */
  StrHeureDebut3 = StrDebut3.substring(0, 2);
  StrMinDebut3 = StrDebut3.substring(3, 5);
  HeureDebut3 = StrHeureDebut3.toInt();
  MinDebut3 = StrMinDebut3.toInt();

  /* End Min */
  EndMin1 = MinDebut1 + duree;
  EndMin2 = MinDebut2 + duree;
  EndMin3 = MinDebut3 + duree;

  /* End Hour */
  EndHour1 = HeureDebut1;
  EndHour2 = HeureDebut2;
  EndHour3 = HeureDebut3;

  if (EndMin1 >= 60) {
    EndMin1 = EndMin1 - 60;
    EndHour1++;
  }
  if (EndMin2 >= 60) {
    EndMin2 = EndMin2 - 60;
    EndHour2++;
  }
  if (EndMin3 >= 60) {
    EndMin3 = EndMin3 - 60;
    EndHour3++;
  }

  /* Formattage interval d'arrosage zone 1 */
  if (StrZone1 == "on")
    sprintf(IntervalZone1, "%s"
                           "%s"
                           "%d"
                           "%s"
                           "%d",
            StrDebut1, "-", EndHour1, ":", EndMin1);
  else
    sprintf(IntervalZone1, "%s", " ");

  /* Formattage interval d'arrosage zone 2 */
  if (StrZone2 == "on")
    sprintf(IntervalZone2, "%s"
                           "%s"
                           "%d"
                           "%s"
                           "%d",
            StrDebut2, "-", EndHour2, ":", EndMin2);
  else
    sprintf(IntervalZone2, "%s", " ");

  /* Formattage interval d'arrosage zone 3 */
  if (StrZone3 == "on")
    sprintf(IntervalZone3, "%s"
                           "%s"
                           "%d"
                           "%s"
                           "%d",
            StrDebut3, "-", EndHour3, ":", EndMin3);
  else
    sprintf(IntervalZone3, "%s", " ");

  /* Récupération du mois et de l'année actuelle sur le server NTP */
  Month = timeinfo.tm_mon + 1;
  Year = timeinfo.tm_year + 1900;

  /* Génération du calendrier du mois en cours */
  int premierJour = calculerPremierJour(Year, Month);

  HTMLtabl = "";
  HTMLtabl += "<h2>Planning d'arrosage</h2> <table> <thead><tr> <th>Lun</th> <th>Mar</th> <th>Mer</th> <th>Jeu</th> <th>Ven</th> <th>Sam</th> <th>Dim</th> </tr> </thead> <tbody>";
  int tablDay = 1;
  int offset = 1;
  int numberOfDays = getNumberOfDays(Year, Month);

  int MaxIter;
  if (premierJour == 6 && numberOfDays == 30)
    MaxIter = 7;
  else
    MaxIter = 6;

  for (int a = 1; a < MaxIter; a++) {
    HTMLtabl += "<tr>";
    for (int i = 1; i < 8; i++) {
      int modulo = tablDay % recurrence;
      if (premierJour >= offset || tablDay > numberOfDays) {
        HTMLtabl += "<td></td>";
      } else if (modulo == 0 && tablDay >= StartDay && tablDay <= EndDay) {
        HTMLtabl += "<td class='highlight'>";  // mettre la case en surbrillance
        HTMLtabl += tablDay;
        HTMLtabl += "</td>";
        tablDay++;
      } else {
        HTMLtabl += "<td>";
        HTMLtabl += tablDay;
        HTMLtabl += "</td>";
        tablDay++;
      }
      offset++;
    }
    HTMLtabl += "</tr>";
  }
  server.sendHeader("Location", "/accueil");
  server.send(302, "text/plain", "");  // Envoi d'un code de statut 302 (Redirection) sans contenu
}

/*

...............................PAGES WEB...............................

*/
/* Code HTML de la page d'accueil */
void accueil() {
  String page = "<!DOCTYPE html> <html> <head> <link rel=\"icon\" type=\"image/png\" href=\"https://static.vecteezy.com/system/resources/previews/021/432/957/non_2x/seedling-and-watering-png.png\"> <link rel=\"apple-touch-icon\" href=\"https://static.vecteezy.com/system/resources/previews/021/432/957/non_2x/seedling-and-watering-png.png\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /> <title>Automatic sprinkler</title> <style> .popup { position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); width: 300px; height: 130px; background-color: white; border: 1px solid black; padding: 20px; display: none; } body { font-family: Arial, sans-serif; background-image: url(\"https://img.freepik.com/vecteurs-libre/fond-geometrique-blanc-dore-realiste_79603-2032.jpg?w=2000\"); /* Remplacez par le chemin vers votre image */ background-repeat: no-repeat; background-size: cover; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; font-weight: 700 ; } h3 { font-size: 30px; margin-bottom: 20px; color: #4CAF50; text-align: center; font-weight: 700 ; } h4 { margin-bottom: 10px; margin-right: 20px; font-weight: normal; text-align: center; } .container { width: 90%; max-width: 700px; min-height: 100vh; } .card { background-color: #fff; border-radius: 10px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); padding: 20px; margin-top: 20px; max-width: 700px; border: 1px solid black; } label { display: block; margin-bottom: 10px; color: #666666; } input[type=\"number\"], input[type=\"date\"], input[type=\"time\"] { width: 100%; padding: 8px; border: 1px solid #cccccc; border-radius: 3px; font-size: 12px; margin-bottom: 10px; } input[type=\"checkbox\"] { margin-left: 10px; margin-right: 10px; } .zone { margin-bottom: 10px; margin-right: 20px; font-weight: normal; } input[type=\"submit\"] { width: 150px; height: 30px; background-color: #4CAF50; text-align: center; font-family: Arial, sans-serif; font-size: 15px; color: white; } h2 { font-size: 18px; margin-bottom: 20px; color: #333; } .button { font-size: 14px; background-color: white; color: black; padding: 10px 20px; border: 1px solid rgba(128, 128, 128, 0.25); width: 100px; cursor: pointer; display: flex; justify-content: center; } .button.green { background-color: #4CAF50; color: white; } </style> <script> function showNotification() { alert(\"Le programme d'arrosage a été enregistré !\"); } function checkTimeInput(zoneId, heureDebutId) { var zoneCheckbox = document.getElementById(zoneId).checked; var heureDebutInput = document.getElementById(heureDebutId); if (!zoneCheckbox) { heureDebutInput.value = \"\"; heureDebutInput.disabled = true; } else { heureDebutInput.disabled = false; } } function validateForm(event) { // Vérification des informations essentielles ici var duree = document.getElementById(\"duree\").value; var dateDebut = document.getElementById(\"dateDebut\").value; var dateFin = document.getElementById(\"dateFin\").value; var recurrence = document.getElementById(\"recurrence\").value; // Exemple de vérification pour le champ \"Durée du programme\" if (duree === \"\" || dateDebut === \"\" || dateFin === \"\" || recurrence === \"\") { alert(\"Veuillez renseigner les informations nécessaires\"); event.preventDefault(); // Empêche l'envoi du formulaire } else { alert(\"Le programme a bien été enregistré !\"); } } </script> </head> <body> <div class=\"container\"> <h1>Automatic sprinkler</h1> <div class=\"card\"> <h2>Operating mode :</h2> <form action=\"/MANU\" method=\"post\"> <button id=\"button1\" class=\"button ";

  if (mode == "MANU")
    page += "green";  // mettre en surbrillance le bouton MANU
  page += "\">MANU</button> </form> <form action=\"/AUTO\" method=\"post\"> <button id=\"button2\" class=\"button ";

  if (mode == "AUTO")
    page += "green";  // mettre en surbrillance le bouton AUTO
  page += "\">AUTO</button> </form> <form action=\"/PROG\" method=\"post\"> <button id=\"button3\" class=\"button ";

  if (mode == "PROG")
    page += "green";  // mettre en surbrillance le bouton PROG
  page += "\">PROG</button> </form> </div> ";

  /* Modification de l'interface en fonction du mode selectioné */
  if (mode == "PROG")
    page += "<div class=\"card\"> <form action=\"/submit\" method=\"post\"> <label for=\"duree\">Program duration (in minutes):</label> <input type=\"number\" id=\"duree\" name=\"duree\" > <br><br> <label for=\"dateDebut\">Start date :</label> <input type=\"date\" id=\"dateDebut\" name=\"dateDebut\" > <br><br> <label for=\"dateFin\">End date :</label> <input type=\"date\" id=\"dateFin\" name=\"dateFin\" > <br><br> <label for=\"recurrence\">Recurrence (in days):</label> <input type=\"number\" id=\"recurrence\" name=\"recurrence\" > <br><br> <h2>Garden areas:</h2> <div class=\"zone\"> <label for=\"zone1\"> <input type=\"checkbox\" id=\"zone1\" name=\"zone1\" onchange=\"checkTimeInput('zone1', 'heureDebut1')\"> Zone 1 </label> <input type=\"time\" id=\"heureDebut1\" name=\"heureDebut1\"> </div> <div class=\"zone\"> <label for=\"zone2\"> <input type=\"checkbox\" id=\"zone2\" name=\"zone2\" onchange=\"checkTimeInput('zone2', 'heureDebut2')\"> Zone 2 </label> <input type=\"time\" id=\"heureDebut2\" name=\"heureDebut2\"> </div> <div class=\"zone\"> <label for=\"zone3\"> <input type=\"checkbox\" id=\"zone3\" name=\"zone3\" onchange=\"checkTimeInput('zone3', 'heureDebut3')\"> Zone 3 </label> <input type=\"time\" id=\"heureDebut3\" name=\"heureDebut3\"> </div> <div id=\"popup\" class=\"popup\"> <h3>Information</h3> <h4>Le programme a bien été enregistré</h4> </div> <br><br> <input type=\"submit\" value=\"Send\" onclick=\"validateForm(event)\"> </form> </div>";

  if (mode == "MANU") {
    page += "<div class=\"card\"> <form action=\"/Vanne1\" method=\"post\"> <button id=\"button4\" class=\"button ";
    if (bpVanne1 == 1)
      page += "green";
    page += "\">Vanne 1</button> </form> <br><br> <form action=\"/Vanne2\" method=\"post\"> <button id=\"button5\" class=\"button ";
    if (bpVanne2 == 1)
      page += "green";
    page += "\">Vanne 2</button> </form> <br><br> <form action=\"/Vanne3\" method=\"post\"> <button id=\"button6\" class=\"button ";
    if (bpVanne3 == 1)
      page += "green";
    page += "\">Vanne 3</button> </form> </div> ";
  }
  page += "</body> </html>";
  server.send(200, "text/html", page);  // envoi de la page web au client
}

/* Code HTML de la page error 404 */
void handleNotFound() {
  String page = "<!DOCTYPE html> <html> <head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" c/tent=\"width=device-width, initial-scale=1.0\"> <title>Page introuvable</title> <style> body { font-family: Arial, sans-serif; background-color: #f2f2f2; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; } .container { text-align: center; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; } p { font-size: 18px; color: #666; text-align: center; } </style> </head> <body> <div class=\"container\"> <h1>Erreur 404</h1> <p>La page que vous recherchez est introuvable.</p> </div> </body> </html>";
  server.send(404, "text/html", page);  // envoi de la page web au client + code erreur 404
}

/* Code HTML du dashboard */
void dashboard() {
  /* Gestion de l'affichage du dashboard en fonction du mode selectionné */
  if (StrZone1 != "on" && StrZone2 != "on" && StrZone3 != "on" && mode == "PROG") {
    String page = "<!DOCTYPE html> <html> <head> <link rel='icon' type='image/png' href='https://img.freepik.com/free-icon/pie-chart_318-372376.jpg'> <link rel='apple-touch-icon' href='https://img.freepik.com/free-icon/pie-chart_318-372376.jpg'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>Dashboard</title> <style> body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; } .container { width: 90%; max-width: 700px; min-height: 100vh; } .card { background-color: #fff; border-radius: 10px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); padding: 20px; margin: 0 auto; max-width: 700px; border: 1px solid black; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; font-weight: 700 ; } h2 { font-size: 18px; margin-bottom: 20px; color: #333; text-align: center; } h3 {font-size: 24px;margin-bottom: 20px;color: #ff0000;text-align: center;} p { font-size: 18px; margin-bottom: 20px; color: #666; } .footer p { text-align: center; margin-top: 20px; font-size: 14px; } </style> <script src='https://unpkg.com/@lottiefiles/lottie-player@latest/dist/lottie-player.js'></script><body><div class='container'><h1>Tableau de bord</h1><div class='card'><h3>Erreur</h3><h2>Aucun programme enregistré</h2><p>Pour créer votre premier programme: <a href='http://";
    page += "192.168.1.35";
    page += "/accueil'>Cliquez ici</a></p><lottie-player src='https://assets5.lottiefiles.com/packages/lf20_zsLhI1gTMs.json'  background='transparent'  speed='0.7'  style='width: 300px; margin: 0 auto; height: 300px;'  loop  autoplay></lottie-player></head>";
    page += "<p>Mode: ";
    page += mode;
    page += "</p> ";
    page += "<p>Software version: ";
    page += version_logiciel;
    page += "</p> ";
    page += "</div><div class='footer'><p style='font-size: 14px;'>Développé par Kyrian BUNEL - 2023</p></div></div></body></html>";
    server.send(200, "text/html", page);  // envoi de la page web au client
  } else if (mode == "AUTO") {            // affichage en mode AUTO
    String page = "<!DOCTYPE html> <html> <head> <link rel=\"icon\" type=\"image/png\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <link rel=\"apple-touch-icon\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv=\"refresh\" content=\"1\"> <title>Dashboard</title> <style> body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; } .rectangleON { width: 150px; height: 30px; background-color: #4CAF50; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleOFF { width: 150px; height: 30px; background-color: #ff0d00; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleDISABLED { width: 150px; height: 30px; background-color: #860063; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .container { width: 90%; max-width: 700px; min-height: 100vh; } .card { background-color: #fff; border-radius: 10px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); padding: 20px; margin: 0 auto; max-width: 700px; border: 1px solid black; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; font-weight: 700 ; } h2 { font-size: 18px; margin-bottom: 20px; color: #333; text-align: center; } p { font-size: 18px; margin-bottom: 10px; color: #666; } table { border-collapse: collapse; width: 100%; margin-bottom: 20px; } th, td { text-align: center; padding: 8px; } th { background-color: #333; color: white; } td { border: 1px solid #ddd; } .today { background-color: #4CAF50; color: white; } .footer p { text-align: center; margin-top: 20px; font-size: 14px; } .highlight { background-color: #4CAF50; color: white; } </style> </head> <body> <div class=\"container\"> <h1>Tableau de bord</h1> <div class=\"card\"> ";
    page += " <h2>Météo</h2>";
    page += "<p>Tepmérature: ";
    page += temperature;
    page += " °C </p> ";
    page += "<p>Humidity: ";
    page += humidity;
    page += " % </p> ";
    page += "<p>Rain sensor: ";
    page += pluie;
    page += " </p> ";
    page += "<p>Brightness: ";
    page += luminosite;
    page += " W/m^2 </p> ";
    page += " <h2>Analyse du sol</h2>";
    page += "<p>Soil moisture Zone 1: ";
    page += MoistureSensor1;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 2: ";
    page += MoistureSensor2;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 3: ";
    page += MoistureSensor3;
    page += " % </p> ";
    page += " <h2>Etat des capteurs</h2> <p>Local date: ";
    page += StrDate;
    page += "</p> <p>Local time: ";
    page += StrHour;
    page += "</p> <p>Memory usage: ";
    page += usagePercentage;
    page += " %";
    page += "<p>Mode: ";
    page += mode;
    page += "</p> ";
    page += "<p>Software version: ";
    page += version_logiciel;
    page += "</p> ";
    page += "</div> <div class=\"footer\"> <p style=\"font-size: 14px;\">Développé par Kyrian BUNEL - 2023</p> </div> </div> </body> </html>";
    server.send(200, "text/html", page);  // envoi de la page web au client
  } else if (mode == "PROG") {            // affichage en mode PROG
    String page = "<!DOCTYPE html> <html> <head> <link rel=\"icon\" type=\"image/png\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <link rel=\"apple-touch-icon\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv=\"refresh\" content=\"1\"> <title>Dashboard</title> <style> body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; } .rectangleON { width: 150px; height: 30px; background-color: #4CAF50; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleOFF { width: 150px; height: 30px; background-color: #ff0d00; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleDISABLED { width: 150px; height: 30px; background-color: #860063; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .container { width: 90%; max-width: 700px; min-height: 100vh; } .card { background-color: #fff; border-radius: 10px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); padding: 20px; margin: 0 auto; max-width: 700px; border: 1px solid black; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; font-weight: 700 ; } h2 { font-size: 18px; margin-bottom: 20px; color: #333; text-align: center; } p { font-size: 18px; margin-bottom: 10px; color: #666; } table { border-collapse: collapse; width: 100%; margin-bottom: 20px; } th, td { text-align: center; padding: 8px; } th { background-color: #333; color: white; } td { border: 1px solid #ddd; } .today { background-color: #4CAF50; color: white; } .footer p { text-align: center; margin-top: 20px; font-size: 14px; } .highlight { background-color: #4CAF50; color: white; } </style> </head> <body> <div class=\"container\"> <h1>Tableau de bord</h1> <div class=\"card\"> <h2>Current program</h2> <p>Start date: ";
    page += StrDateDebut;
    page += "</p> <p>End date: ";
    page += StrDateFin;
    page += "</p> <p>Watering duration: ";
    page += StrDuree;
    page += " min";
    page += "</p> <p>Recurrence: ";
    page += StrRecurrence;
    page += " days";
    page += HTMLtabl;
    page += "</tbody> </table>";
    page += "</p> <h2>Etat des vannes</h2> ";
    page += "<p>Zone 1: ";
    page += IntervalZone1;
    page += "</p> ";
    page += HtmlZone1;
    page += " <p>Zone 2: ";
    page += IntervalZone2;
    page += "</p> ";
    page += HtmlZone2;
    page += " <p>Zone 3: ";
    page += IntervalZone3;
    page += "</p> ";
    page += HtmlZone3;
    page += " <h2>Etat des capteurs</h2> <p>Local date: ";
    page += StrDate;
    page += "</p> <p>Local time: ";
    page += StrHour;
    page += "<p>Temperature: ";
    page += temperature;
    page += " °C </p> ";
    page += "<p>Humidity: ";
    page += humidity;
    page += " % </p> ";
    page += "<p>Rain sensor: ";
    page += pluie;
    page += " </p> ";
    page += "<p>Soil moisture Zone 1: ";
    page += MoistureSensor1;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 2: ";
    page += MoistureSensor2;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 3: ";
    page += MoistureSensor3;
    page += " % </p> ";
    page += "<p>Brightness: ";
    page += luminosite;
    page += " W/m^2 </p> ";
    page += "</p> <p>Memory usage: ";
    page += usagePercentage;
    page += " %";
    page += "<p>Mode: ";
    page += mode;
    page += "</p> ";
    page += "<p>Software version: ";
    page += version_logiciel;
    page += "</p> ";
    page += "</div> <div class=\"footer\"> <p style=\"font-size: 14px;\">Développé par Kyrian BUNEL - 2023</p> </div> </div> </body> </html>";
    server.send(200, "text/html", page);  // envoi de la page web au client
  } else {                                // affichage en mode MANU et AUTO
    String page = "<!DOCTYPE html> <html> <head> <link rel=\"icon\" type=\"image/png\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <link rel=\"apple-touch-icon\" href=\"https://img.freepik.com/free-icon/pie-chart_318-372376.jpg\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv=\"refresh\" content=\"1\"> <title>Dashboard</title> <style> body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; } .rectangleON { width: 150px; height: 30px; background-color: #4CAF50; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleOFF { width: 150px; height: 30px; background-color: #ff0d00; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .rectangleDISABLED { width: 150px; height: 30px; background-color: #860063; text-align: center; line-height: 30px; font-family: Arial, sans-serif; color: white; } .container { width: 90%; max-width: 700px; min-height: 100vh; } .card { background-color: #fff; border-radius: 10px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); padding: 20px; margin: 0 auto; max-width: 700px; border: 1px solid black; } h1 { font-size: 36px; margin-bottom: 25px; color: #4CAF50; text-align: center; font-weight: 700 ; } h2 { font-size: 18px; margin-bottom: 20px; color: #333; text-align: center; } p { font-size: 18px; margin-bottom: 10px; color: #666; } table { border-collapse: collapse; width: 100%; margin-bottom: 20px; } th, td { text-align: center; padding: 8px; } th { background-color: #333; color: white; } td { border: 1px solid #ddd; } .today { background-color: #4CAF50; color: white; } .footer p { text-align: center; margin-top: 20px; font-size: 14px; } .highlight { background-color: #4CAF50; color: white; } </style> </head> <body> <div class=\"container\"> <h1>Tableau de bord</h1> <div class=\"card\">";
    page += "</p> <h2>States of the valves</h2> ";
    page += "<p>Zone 1: ";
    page += IntervalZone1;
    page += "</p> ";
    page += HtmlZone1;
    page += " <p>Zone 2: ";
    page += IntervalZone2;
    page += "</p> ";
    page += HtmlZone2;
    page += " <p>Zone 3: ";
    page += IntervalZone3;
    page += "</p> ";
    page += HtmlZone3;
    page += " <h2>Sensor status</h2> <p>Local date: ";
    page += StrDate;
    page += "</p> <p>Local hour: ";
    page += StrHour;
    page += "<p>Temperature: ";
    page += temperature;
    page += " °C </p> ";
    page += "<p>Humidity: ";
    page += humidity;
    page += " % </p> ";
    page += "<p>Rain sensor: ";
    page += pluie;
    page += "</p> ";
    page += "<p>Soil moisture Zone 1: ";
    page += MoistureSensor1;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 2: ";
    page += MoistureSensor2;
    page += " % </p> ";
    page += "<p>Soil moisture Zone 3: ";
    page += MoistureSensor3;
    page += " % </p> ";
    page += "<p>Brightness: ";
    page += luminosite;
    page += " W/m^2 </p> ";
    page += "</p> <p>Memory usage: ";
    page += usagePercentage;
    page += " %";
    page += "<p>Mode: ";
    page += mode;
    page += "</p> ";
    page += "<p>Software version: ";
    page += version_logiciel;
    page += "</p> ";
    page += "</div> <div class=\"footer\"> <p style=\"font-size: 14px;\">Développé par Kyrian BUNEL - 2023</p> </div> </div> </body> </html>";
    server.send(200, "text/html", page);  // envoi de la page web au client
  }
}

/*

...............................SETUP...............................

*/
void setup() {

  size_t freeMem = ESP.getFreeHeap();  // calcul de la mémoire restante

  size_t totalMem = 400000;  // Mémoire totale

  usagePercentage = (1.0 - (float)freeMem / totalMem) * 100.0;  // Calcul du pourcentage d'utilisation de la mémoire

  Serial.begin(115200);  // Initialisation du moniteur série
  dht.begin();           // Initialisation du capteur de température et d'humidité

  pinMode(36, INPUT);  // Initialisation du capteur de pluie en entrée
  pinMode(39, INPUT);  // Initialisation du capteur luminosité en entrée
  pinMode(35, INPUT);  // Initialisation du capteur d'humidité dans le sol en entrée

  pinMode(vanne1, OUTPUT);  // Initialisation de la vanne 1 en sortie
  pinMode(vanne2, OUTPUT);  // Initialisation de la vanne 2 en sortie
  pinMode(vanne3, OUTPUT);  // Initialisation de la vanne 3 en sortie

  digitalWrite(vanne1, HIGH);  // fermeture de la vanne 1 (mesure de sécurité)
  digitalWrite(vanne2, HIGH);  // fermeture de la vanne 2 (mesure de sécurité)
  digitalWrite(vanne3, HIGH);  // fermeture de la vanne 3 (mesure de sécurité)

  /* Initialisation du WiFi */
  WiFiManager wm;
  //wm.resetSettings(); // Reset the Wi-Fi configuration after eatch reboot
  wm.setDebugOutput(false);

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("ArrosageAUTO_001");  // anonymous ap

  if (!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    WiFi.setHostname("Arrosage-automatique");  // définition du nom du périférique
    Serial.print("\nConnected to the wifi network: ");
    Serial.println(ssid);
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());  // affichage de l'adresse IP locale sur le moniteur série
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // Configuration du seveur NTP

  /* Initialisation des sous-répertoires */
  server.on("/accueil", accueil);
  server.on("/dashboard", dashboard);
  server.on("/submit", handleSubmit);
  server.on("/MANU", handleMANU);
  server.on("/AUTO", handleAUTO);
  server.on("/PROG", handlePROG);
  server.on("/Vanne1", handleVanne1);
  server.on("/Vanne2", handleVanne2);
  server.on("/Vanne3", handleVanne3);
  server.onNotFound(handleNotFound);
  server.begin();


  sprintf(HtmlZone1, "%s", HtmlDISABLED);
  sprintf(HtmlZone2, "%s", HtmlDISABLED);
  sprintf(HtmlZone3, "%s", HtmlDISABLED);

  /* Code provenat de la bibliothèque "ArduinoOTA" */
  ArduinoOTA.setHostname("Arrosage_automatique");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();  // Activation de l'OTA

  Serial.println("Serveur web actif!");  // Affichage dans le moniteur série une fois le SETUP terminé
}

/*

...............................LOOP...............................

*/
void loop() {

  server.handleClient();  // Maintenir le client actif (pages web)
  ArduinoOTA.handle();    // Maintenir le client actif (OTA)

  pluie = analogRead(CapteurPluie);  // lecture de la valeur du capteur de pluie + gestion du string pour affichage au dashboard
  if (pluie >> 1)
    StrPluie = "True";
  else
    StrPluie = "False";

  MoistureSensor1 = map(analogRead(35), 0, 4095, 100, 0);  // lecture de la valeur de capteur de pluie + convertion 0,4095->0,100 (%)
  MoistureSensor2 = map(analogRead(33), 0, 4095, 100, 0);  // lecture de la valeur de capteur de pluie + convertion 0,4095->0,100 (%)
  MoistureSensor3 = map(analogRead(34), 0, 4095, 100, 0);  // lecture de la valeur de capteur de pluie + convertion 0,4095->0,100 (%)
  //luminosite = map(analogRead(39), 0, 4095, 10000, 0);     // lecture de la valeur de capteur luminosité + convertion 0,4095->10000,0 (W/m^2)
  luminosite = analogRead(39);

  humidity = dht.readHumidity();        // Mesure de l'humidité + rectification de l'erreur
  temperature = dht.readTemperature();  // Mesure de la température + rectification de l'erreur

  Hour = timeinfo.tm_hour;         // Récupération de l'heure sur le serveur NTP
  Min = timeinfo.tm_min;           // Récupération des minutes sur le serveur NTP
  Sec = timeinfo.tm_sec;           // Récupération des secondes sur le serveur NTP
  Day = timeinfo.tm_mday;          // Récupération du jour sur le serveur NTP
  Month = timeinfo.tm_mon + 1;     // Récupération du mois sur le serveur NTP
  Year = timeinfo.tm_year + 1900;  // Récupération de l'année sur le serveur NTP

  sprintf(StrHour, "%d"
                   "%s"
                   "%d"
                   "%s"
                   "%d",
          Hour, ":", Min, ":", Sec);  // Formattage de l'horloge pour affichage sur le dashboard
  sprintf(StrDate, "%d"
                   "%s"
                   "%d"
                   "%s"
                   "%d",
          Day, "-", Month, "-", Year);  // Formattage de la date pour affichage sur le dashboard

  if (!getLocalTime(&timeinfo)) {  // Impossible d'obtenir la date et l'heure
    Serial.println("Failed to obtain time");
  }

  /* Conditions mode PROG */
  if (recurrence != 0 && mode == "PROG") {                                                                                                          // vérification du mode et de la récurence (div 0)
    if (Day >= StartDay && Month >= StartMonth && Year >= StartYear) {                                                                              // vérification de la date d'arrosage
      if (StrZone1 == "on" && Day % recurrence == 0 && Hour >= HeureDebut1 && Hour <= EndHour1 && Min >= MinDebut1 && Min < EndMin1 && pluie == 1)  // vérification de l'heure d'arrogage, de la selection de la zone & capteur de pluie
      {
        digitalWrite(vanne1, LOW);
        sprintf(HtmlZone1, "%s", HtmlON);
      } else {
        sprintf(HtmlZone1, "%s", HtmlOFF);
        digitalWrite(vanne1, HIGH);
      }
      if (StrZone2 == "on" && Day % recurrence == 0 && Hour >= HeureDebut2 && Hour <= EndHour2 && Min >= MinDebut2 && Min < EndMin2 && pluie == 1)  // vérification de l'heure d'arrogage, de la selection de la zone & capteur de pluie
      {
        digitalWrite(vanne2, LOW);
        sprintf(HtmlZone2, "%s", HtmlON);
      } else {
        sprintf(HtmlZone2, "%s", HtmlOFF);
        digitalWrite(vanne2, HIGH);
      }
      if (StrZone3 == "on" && Day % recurrence == 0 && Hour >= HeureDebut3 && Hour <= EndHour3 && Min >= MinDebut3 && Min < EndMin3 && pluie == 1)  // vérification de l'heure d'arrogage, de la selection de la zone & capteur de pluie
      {
        digitalWrite(vanne3, LOW);
        sprintf(HtmlZone3, "%s", HtmlON);
      } else {
        sprintf(HtmlZone3, "%s", HtmlOFF);
        digitalWrite(vanne3, HIGH);
      }
    }

    if (Min >= EndMin1 && StrZone1 == "on") {
      digitalWrite(vanne1, HIGH);
      sprintf(HtmlZone1, "%s", HtmlOFF);
    }
    if (Min >= EndMin2 && StrZone2 == "on") {
      digitalWrite(vanne2, HIGH);
      sprintf(HtmlZone2, "%s", HtmlOFF);
    }
    if (Min >= EndMin3 && StrZone3 == "on") {
      digitalWrite(vanne3, HIGH);
      sprintf(HtmlZone3, "%s", HtmlOFF);
    }

    if (StrZone1 == "") {                      // Zone 1 non sélectionnée
      sprintf(HtmlZone1, "%s", HtmlDISABLED);  // afficher comme désactivée dans le dashboard
    }
    if (StrZone2 == "") {                      // Zone 2 non sélectionnée
      sprintf(HtmlZone2, "%s", HtmlDISABLED);  // afficher comme désactivée dans le dashboard
    }
    if (StrZone3 == "") {                      // Zone 3 non sélectionnée
      sprintf(HtmlZone3, "%s", HtmlDISABLED);  // afficher comme désactivée dans le dashboard
    }
  }

  /* Conditions mode MANU */
  else if (mode == "MANU") {  // vérification du mode
    if (bpVanne1 == 1) {
      digitalWrite(vanne1, LOW);
      sprintf(HtmlZone1, "%s", HtmlON);
    } else {
      digitalWrite(vanne1, HIGH);
      sprintf(HtmlZone1, "%s", HtmlOFF);
    }
    if (bpVanne2 == 1) {
      digitalWrite(vanne2, LOW);
      sprintf(HtmlZone2, "%s", HtmlON);
    } else {
      digitalWrite(vanne2, HIGH);
      sprintf(HtmlZone2, "%s", HtmlOFF);
    }
    if (bpVanne3 == 1) {
      digitalWrite(vanne3, LOW);
      sprintf(HtmlZone3, "%s", HtmlON);
    } else {
      digitalWrite(vanne3, HIGH);
      sprintf(HtmlZone3, "%s", HtmlOFF);
    }
  }

  /* Conditions mode AUTO (en cours) */
  else if (mode == "AUTO") {  // vérification du mode
    if (ModeAuto == 1 && pluie <= 200 && MoistureSensor1 <= SeuilHumidite && temperature < SeuilTemperatureMAX && temperature > SeuilTemperatureMIN && humidity < SeuilHumidity) {
      digitalWrite(vanne1, LOW);
      sprintf(HtmlZone1, "%s", HtmlON);
      TimerAUTO++;
      JourArrosage = Day;
    }
    if (TimerAUTO >= delayAUTO) {
      ModeAuto = 0;
      TimerAUTO = 0;
      digitalWrite(vanne1, HIGH);
      sprintf(HtmlZone1, "%s", HtmlOFF);
    }
    if (Day - JourArrosage >= 2)
      ModeAuto = 1;
  }
  /* Désactivation des vannes si non */
  else {
    digitalWrite(vanne1, HIGH);
    sprintf(HtmlZone1, "%s", HtmlDISABLED);
    digitalWrite(vanne2, HIGH);
    sprintf(HtmlZone2, "%s", HtmlDISABLED);
    digitalWrite(vanne3, HIGH);
    sprintf(HtmlZone3, "%s", HtmlDISABLED);
  }

  if (en == 1 && Min == 0 && Year != 1900) {
    sendData();
    en = 0;
  }
  if (Min == 2)
    en = 1;

  delay(1);  // ajout d'un délais d'une ms pour la stabilité du système
}
