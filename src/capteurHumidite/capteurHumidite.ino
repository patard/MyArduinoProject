//****************************************************
// Mesure analogique et digitale de l'humidité du sol
// Avec le module Humidité Sol
//  TipTopboards.com
//  23 12 2013  demo_hsol
//****************************************************
// Brancher +V -> 5V et GND
// Sortie analogique du capteur -> A0 Arduino
// Sortie digitale du capteur -> pin 3 Arduino (avec seuil)

//On rajoute une LEd témoin sur pin 4

int PinAnalogiqueHumidite = 0;     //Broche Analogique de mesure d'humidité
int PinNumeriqueHumidite = 2;      //Broche Numérique mesure de l'humidité
int PinLed = 3;  //LED témoin de seuilde  sécheresse


int hsol;  //Humidite su sol, mesure analogique
int secheresse;  //0 ou 1 si seuil atteint

void setup() { // Initialisation
  Serial.begin(9600);  //Connection série à 9600 baud
  pinMode(PinAnalogiqueHumidite, INPUT);       //pin A0 en entrée analogique
  pinMode(PinNumeriqueHumidite, INPUT);  //pin 3 en entrée numérique
  // pinMode(PinLed, OUTPUT);   //LED témoin
}

void loop() { //boucle principale
  hsol = analogRead(PinAnalogiqueHumidite); // Lit la tension analogique
  secheresse = digitalRead(PinNumeriqueHumidite);
  Serial.print(hsol); // afficher la mesure
  Serial.print("  ");
  Serial.println(secheresse);  //0 ou 1 si le seuil est dépassé
  //if (secheresse == 1)
  //{
  // digitalWrite(PinLed, HIGH);   // LED allumée
  //}
  //else {
  // digitalWrite(PinLed, LOW);   // LED off
  //}
  delay(1000); //delai entre 2 mesures 20ms
}
