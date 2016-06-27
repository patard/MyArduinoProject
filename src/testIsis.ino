#include <Isis.h>


void setup() {
  // put your setup code here, to run once:
  Isis.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( Isis.hasNewData() )
  {
    Isis.decodeMessage();
  }
}


