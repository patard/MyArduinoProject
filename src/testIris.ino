#include <ArduinoUnit.h>
#include <Wire.h>

#include <Iris.h>

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
  Wire.write(data); // sends one byte
  Wire.endTransmission(); // stop transmitting
  //delay(100); // the Isis is treating message
  // set pin 13 to LOW
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device
  msgId = 3;
  data = 52; // 0x34
  Wire.write(msgId);
  Wire.write(data); // sends one byte
  Wire.endTransmission(); // stop transmitting
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
  Wire.write(data); // sends one byte
  Wire.endTransmission(); // stop transmitting
  // check result
  delay(100);
  value = digitalRead(13);
  Serial.print("testLed13 high ");
  Serial.println (value);
  assertEqual(value, HIGH);
}

test(digitRead)
{
   byte msgId;
  byte data;
  int value;
 
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device
  // encode msg digitalRead on pinNumber
  msgId = 1;
  data = 53; // 0x35
  Wire.write(msgId);
  Wire.write(data); // sends one byte
  Wire.endTransmission(); // stop transmitting

  // step 2: wait for readings to happen
  delay(70);                   // datasheet suggests at least 65 milliseconds ....

  // step 4: request reading from sensor
  Wire.requestFrom(I2C_SLAVE_ADDR, 2);    // request 2 bytes from slave device 


  // step 5: receive reading from sensor
  byte reading[2];
  if (2 <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading) it is the msgId
    //check msgId == DIGITAL_READ_VALUE_MSG_ID
    assertEqual(reading[0], DIGITAL_READ_VALUE_MSG_ID);
    reading[1] = Wire.read(); 
    // decode digitalRead
    int pinNumber = 0;
    int valueRead;
    IrisClass::decodeDigitalRead(reading[1], &pinNumber, &valueRead);
    Serial.println(valueRead);   // print the reading
  }
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
