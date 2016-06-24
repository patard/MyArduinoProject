#include <SPI.h>
#include <RFID.h>

// code pour lire l'UID sur le serial Monitor
/*
RFID monModuleRFID(10, 9);

int UID[5];

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  monModuleRFID.init();

}

void loop()
{
  if (monModuleRFID.isCard()) {
    if (monModuleRFID.readCardSerial()) {
      Serial.print("L'UID est: ");
      for (int i = 0; i <= 4; i++)
      {
        UID[i] = monModuleRFID.serNum[i];
        Serial.print(UID[i], DEC);
        Serial.print(".");
      }
      Serial.println("");
    }
    monModuleRFID.halt();
  }
  delay(1);
}*/

// Code pour carte d'accès


RFID monModuleRFID(10,9);

int UID[5]={};
int MASTERKEY[5]={37,90,210,101,200};

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  monModuleRFID.init();  
}

void loop()
{
    if (monModuleRFID.isCard()) {  
          if (monModuleRFID.readCardSerial()) {        
            Serial.print("L'UID est: ");
            for(int i=0;i<=4;i++)
            {
              UID[i]=monModuleRFID.serNum[i];
              Serial.print(UID[i],DEC);
              Serial.print(".");
            }
            Serial.println("");
          }
          
          if (UID[0] == MASTERKEY[0]
           && UID[1] == MASTERKEY[1]
           && UID[2] == MASTERKEY[2]
           && UID[3] == MASTERKEY[3]
           && UID[4] == MASTERKEY[4])
          {
              Serial.println("acces granted");
          }
          else
          {
              Serial.println("acces refused");
          }    
                
          monModuleRFID.halt();
    }
    delay(1);    
}

