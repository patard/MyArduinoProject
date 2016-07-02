/*
 * This sketch allow to use the Iris library on Arduino.
 * It will receive order for master, interpret them and react as wanted.
 * For an example of used on master device see the testIris sketch.
 */

#include <IrisArduino.h>

void setup() {
  Iris.begin();
}

void loop() {
  if ( Iris.available() )
  {
    Iris.decodeMessage();
  }
}
