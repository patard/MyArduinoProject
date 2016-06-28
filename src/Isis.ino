#include <Isis.h>
void setup() {
  Isis.begin();
}

void loop() {
  if ( Isis.hasNewData() )
  {
    Isis.decodeMessage();
  }
}
