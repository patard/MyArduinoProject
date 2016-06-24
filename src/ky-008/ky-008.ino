int laserPin = 10;

void setup ()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode (laserPin, OUTPUT); // define the digital output interface 13 feet
}

void loop () {
//  digitalWrite (laserPin, HIGH); // Turn Laser On
//  delay (2000); // On For Half a Second
//  Serial.print(" valeur laser ON: ");
//  Serial.println (analogRead(A0) * 5.0 / 1023);
//  digitalWrite (laserPin, LOW); // Turn Laser Off
//  delay (1000); // Off for half a second
//  Serial.print(" valeur laser OFF: ");
//  Serial.println (analogRead(A0) * 5.0 / 1023);


  digitalWrite (laserPin, HIGH); // Turn Laser On
  delay (500); // On For Half a Second
  float valueRead = analogRead(A0) * 5.0 / 1023;
  Serial.println (valueRead);
  if ( valueRead < 4.5 )
  {
    Serial.println ("faisceau coupe");
    digitalWrite (laserPin, LOW); // Turn Laser Off
    delay (1000); // Off for half a second
  }

}
