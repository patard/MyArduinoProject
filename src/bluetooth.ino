#include <SoftwareSerial.h>
SoftwareSerial BT(10, 11);
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 3.3V, GND to GND

// ----------------------------------------------------------------------------------------
// Protocle de communication entre Arduino et commande à distance
// ----------------------------------------------------------------------------------------
// Codage de la direction recu selon la disposition des chiffres sur un pavé numérique
// Un "P" est recu en premier afin de signifier qu'il s'agit d'une information de position
// 7=Avant gauche     8=avant     9=avant droite
// 4=gauche                       6=droite
// 1=arrière gauche   2=arriere   3=arrire droite

// Codage de la vitesse en pourcentage
// Un "S" est recu en premier afin de signifier qu'il s'agit d'une information de vitesse

// tous les ordres sont terminés par un "#"
// ----------------------------------------------------------------------------------------
String speedStart = "S";
String posStart = "P";

#define AVANT_GAUCHE    7
#define AVANT           8
#define AVANT_DROITE    9
#define GAUCHE          4
#define DROITE          6
#define ARRIERE_GAUCHE  1
#define ARRIERE         2
#define ARRIERE_DROITE  3



// Motor left
int motorLeftPinI1 = 2;
int motorLeftPinI2 = 4;
int motorLeftSpeedPin = 3; // Needs to be a PWM pin to be able to control motor speed

// Motor right
int motorRightPinI1 = 8;
int motorRightPinI2 = 7;
int motorRightSpeedPin = 9; // Needs to be a PWM pin to be able to control motor speed

void setup()
{
  // Ouvre la voie série avec l'ordinateur
  Serial.begin(9600);

  // set the data rate for the SoftwareSerial port
  BT.begin(9600);

  //Define L298N Dual H-Bridge Motor Controller Pins
  pinMode(motorLeftPinI1, OUTPUT);
  pinMode(motorLeftPinI2, OUTPUT);
  pinMode(motorLeftSpeedPin, OUTPUT);

  pinMode(motorRightPinI1, OUTPUT);
  pinMode(motorRightPinI2, OUTPUT);
  pinMode(motorRightSpeedPin, OUTPUT);
}

void decodeSpeed(String * msg)
{
  int speedPercent = msg->toInt();
  Serial.print("SPEED (%): ");
  Serial.print(speedPercent);

  int pwmValue =  map(speedPercent, 0, 100, 0, 255);
  Serial.print(" PWM: ");
  Serial.println(pwmValue);
  analogWrite(motorLeftSpeedPin, pwmValue);
  analogWrite(motorRightSpeedPin, pwmValue);
}


void goLeft()
{
  // to go left, only the right wheel must run
  digitalWrite(motorRightPinI1, LOW);
  digitalWrite(motorRightPinI2, HIGH);
  analogWrite(motorRightSpeedPin, 50);
  analogWrite(motorLeftSpeedPin, 0);
}


void goRight()
{
  // to go right, only the left wheel must run
  digitalWrite(motorLeftPinI1, LOW);
  digitalWrite(motorLeftPinI2, HIGH);
  analogWrite(motorLeftSpeedPin, 50);
  analogWrite(motorRightSpeedPin, 0);
}


void goFront()
{
  digitalWrite(motorRightPinI1, LOW);
  digitalWrite(motorRightPinI2, HIGH);
  digitalWrite(motorLeftPinI1, LOW);
  digitalWrite(motorLeftPinI2, HIGH);
}


void goRear()
{
  digitalWrite(motorRightPinI1, HIGH);
  digitalWrite(motorRightPinI2, LOW);
  digitalWrite(motorLeftPinI1, HIGH);
  digitalWrite(motorLeftPinI2, LOW);
}


void decodePos(String * msg)
{
  switch (msg->toInt())
  {
    case AVANT_GAUCHE:  Serial.println("AVANT_GAUCHE"); break;
    case AVANT:
      Serial.println("AVANT");
      goFront();
      break;
    case AVANT_DROITE:  Serial.println("AVANT_DROITE"); break;
    case GAUCHE:
      Serial.println("GAUCHE");
      goLeft();
      break;
    case DROITE:
      Serial.println("DROITE");
      goRight();
      break;
    case ARRIERE_GAUCHE: Serial.println("ARRIERE_GAUCHE"); break;
    case ARRIERE:
      Serial.println("ARRIERE");
      goRear();
      break;
    case ARRIERE_DROITE: Serial.println("ARRIERE_DROITE"); break;
    default: Serial.println("Position incomprise"); break;
  }
}

void loop()
{
  if (BT.available())
    // if text arrived in from BT serial...
  {
    String msgRead = BT.readStringUntil('#');


    if (msgRead.startsWith(speedStart))
    {
      msgRead.remove(0, 1);
      decodeSpeed(&msgRead);
    }
    else if (msgRead.startsWith(posStart))
    {
      msgRead.remove(0, 1);
      decodePos(&msgRead);
    }
    else
    {
      Serial.print("msg non compris: " );
      Serial.print(msgRead);
      Serial.println();
    }


  }
}
