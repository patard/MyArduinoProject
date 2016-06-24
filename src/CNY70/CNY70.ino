
int inPin = 3; // pino 4
int pinAnalogique = A0;
int valor = 0; //
void setup() {

  Serial.begin(9600);
  pinMode(inPin, INPUT);

}
void loop() {
  valor = digitalRead(inPin); //guardar valor lido na variavel valor
  Serial.print(" valeur ");

  Serial.print( analogRead(pinAnalogique));

  if (valor == 1 )
  {
    Serial.println("  BLANC");
  }
  else
  {
    Serial.println("  NOIR");
  }

  //Serial.println(valor);//mostra o valor lido no serial monitor
  delay (500);

}
