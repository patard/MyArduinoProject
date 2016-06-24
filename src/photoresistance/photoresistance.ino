
const char led = 2;            //Une LED pour indiquer s'il fait jour
const char capteur = 0; //broche A0 sur laquelle va être connecté le pont diviseur de tension
 
float tension = 0;             //variable qui va enregistrer la tension lue en sortie du capteur
float seuilObscurite = 1.5;    // valeur en V, seuil qui détermine le niveau auquel l'obscurité est présente
 
void setup()
{
    //définition des broches utilisées
    pinMode(led, OUTPUT);
 
    Serial.begin(9600); //la voie série pour monitorer
}

void loop()
{
    tension = (analogRead(capteur) * 5.0) / 1024;  // conversion de cette valeur en tension
 
    if(tension >= seuilObscurite)
    {
        digitalWrite(led, LOW); //On allume la LED
    }
    else
    {
        digitalWrite(led, HIGH); //On éteint la LED
    }
    // envoie vers l'ordinateur, via la liaison série, la valeur de la tension lue
    Serial.print("Tension = ");
    Serial.print(tension);
    Serial.println(" V");
 
    delay(500);  // délai pour ne prendre des mesures que toutes les demi-secondes
}
