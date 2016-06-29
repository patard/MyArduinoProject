#include <Iris.h>
void setup() {
  Iris.begin();
}

void loop() {
  if ( Iris.newDataAvailable() )
  {
    Iris.decodeMessage();
  }
}
