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
