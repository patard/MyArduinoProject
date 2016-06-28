#include <ArduinoUnit.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x05

test(ok) 
{
  int x=3;
  int y=3;
  assertEqual(x,y);
}

test(bad)
{
  int x=3;
  int y=3;
  assertNotEqual(x,y);
}

test(led13)
{
  byte msgId;
  byte data;

  // set pin 13 to OUTPUT
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device 
  msgId = 1;
  data = 53; // 0x35
  Wire.write(msgId);       
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  // set pin 13 to HIGH
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device 
  msgId = 3;
  data = 53; // 0x35
  Wire.write(msgId);       
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting
}

void setup()
{
   Wire.begin(); // join i2c bus (address optional for master)
}

void loop()
{
  Test::run();
}


