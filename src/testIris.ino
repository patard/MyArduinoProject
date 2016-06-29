#include <ArduinoUnit.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x05

test(ok)
{
  int x = 3;
  int y = 3;
  assertEqual(x, y);
}

/*
  test(bad)
  {
  int x = 3;
  int y = 3;
  assertNotEqual(x, y);
  // assertEqual(x, y);
  }*/


test(led13)
{
  byte msgId;
  byte data;
  int value;

  // set pin 13 to OUTPUT
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device
  msgId = 1;
  data = 53; // 0x35
  Wire.write(msgId);
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  //delay(100); // the Isis is treating message

  // set pin 13 to LOW
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device
  msgId = 3;
  data = 52; // 0x34
  Wire.write(msgId);
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  // check result
  delay(100);
  value = digitalRead(13);
  Serial.print("testLed13 low ");
  Serial.println (value);
  assertEqual(value, LOW);

  // set pin 13 to HIGH
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device
  msgId = 3;
  data = 53; // 0x35
  Wire.write(msgId);
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  // check result
  delay(100);
  value = digitalRead(13);
  Serial.print("testLed13 high ");
  Serial.println (value);
  assertEqual(value, HIGH);
}

void setup()
{
  pinMode(13, INPUT_PULLUP);
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
  Serial.println("testIris pret");
}

void loop()
{
  Test::run();
}



