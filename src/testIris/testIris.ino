/*
   The aim of this sketch is to test the Iris library.
   It must be downloaded in an Arduino Nano or Uno.
   The ArduinoUnit library is needed (https://github.com/mmurdoch/arduinounit)

   Two arduino needed :
   - one tester on which this sketch is downloaded
   - one Arduino under test (AUT) where the Iris main sketch is downloaded

   They are connected via I2C:

   Arduino tester pin number          Arduino under test pin number
            A4  --------------------------------  A4
            A5  --------------------------------  A5
            GND -------------------------------- GND
            D13 -------------------------------- D13 (test digitalWrite)
            D12 -------------------------------- D12 (test digitalRead)
            A0  -------- R1 = 1kOhm ------------ D3  (test analogWrite)
                              |
                              |----------------- A0  (test analogRead)
                              |
                         C1 = 470 uF  (make a RC circuit to filter PWM)
                              |
                              |
                             GND




   For an explanation of the RC circuit (for example) :
   cf http://www.instructables.com/id/Arduino-RC-Circuit-PWM-to-analog-DC/step6/Arduino-RC-circuit/

*/


#include <ArduinoUnit.h>
#include <Wire.h>

#include <IrisArduino.h>

#define I2C_SLAVE_ADDR 0x05 // address of the AUT on the I2C

int testNumber;

/*
  test(bad)
  {
  int x = 3;
  int y = 3;
  assertNotEqual(x, y);
  // assertEqual(x, y);
  }*/


test(analRead)
{
  Serial.println(F("ENTRY analogRead"));

  byte reading[ANALOG_READ_VALUE_MSG_SIZE];
 // int pinNumberMaster = A0;
  int pinNumberAutWrite = 3; //Arduino under test => write PWM
  int pinNumberAutRead = 0; //Arduino under test A0
  // to reuse the RC circuit
  int valueToSet = 125;
  int valueReadOnMaster;
  int valueReadOnAut;

  // send DEBUG Msg
  Iris.debugMsgReq(testNumber);


  // ---- Write analog value on pin on AUT
  Serial.println( F("Write analog value on AUT"));
  Iris.analogWriteReq(pinNumberAutWrite, valueToSet);
  delay(1000); // let RC capacity to charge

  // -- read on Analog input on master // TODO compare with analogReadReq
  valueReadOnMaster = analogRead(A0);
  Serial.println(valueReadOnMaster);
  // Serial.println(map(valueRead, 0, 1023, 0, 255));
  Serial.println("--");

  delay(100);
  // -- request analog read on AUT
  Iris.analogReadReq(pinNumberAutRead);

  
  // ---- check data returned
  Serial.println( F("Check value read on AUT"));
  if (ANALOG_READ_VALUE_MSG_SIZE <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading) it is the msgId
    assertEqual(reading[0], ANALOG_READ_VALUE_MSG_ID);
    reading[1] = Wire.read();
    reading[2] = Wire.read();
 
    int pinNumber = 0;
    
    Serial.println(reading[1]);
    Serial.println(reading[2]);
     IrisClass::decodeAnalogReadValue(&reading[1], &pinNumber, &valueReadOnAut);
    /*  Serial.println(pinNumber);
      // assertEqual(pinNumber, pinNumberAutRead);
      Serial.println(valueRead);
      // assertEqual(valueRead, valueToSet);*/
  }


  assertEqual(1, 1);

  Serial.println(F("EXIT analogRead"));
  Serial.println(F("---------------------------"));
  testNumber++;
}

/*
  test(analWrite) // TODO assert
  {
  Serial.println(F("ENTRY analogWrite"));

  int pinNumberMaster = A0;
  int pinNumberAut = 3; //Arduino under test
  int valueToSet = 125;
  int valueRead;

  // send DEBUG Msg
  Iris.debugMsgReq(testNumber);

  // ---- Write analog value on pin on AUT
  Serial.println( F("Write analog value on AUT"));
  Iris.analogWriteReq(pinNumberAut, valueToSet);
  delay(1000); // let RC capacity to charge

  // -- read on Analog input
  valueRead = analogRead(pinNumberMaster);
  Serial.println(valueRead);
  // Serial.println(map(valueRead, 0, 1023, 0, 255));
  Serial.println("--");

  valueToSet = 62;
  Iris.analogWriteReq(pinNumberAut, valueToSet);
  delay(1000);
  valueRead = analogRead(pinNumberMaster);
  Serial.println(valueRead);
  //Serial.println(map(valueRead, 0, 1023, 0, 255));

  assertEqual(1, 1);

  Serial.println(F("EXIT analogWrite"));
  Serial.println("---------------------------");
  testNumber++;
  }*/

/*
  test(digitWrite)
  {
  int value;

  Serial.println(F("ENTRY digitalWrite"));

  int pinNumberMaster = 13;
  int pinNumberAut = 13; //Arduino under test

  // send DEBUG Msg
  Iris.debugMsgReq(testNumber);

  // ---- Set pin on AUT to OUTPUT MODE
  Serial.println( F("Set pin on AUT to OUTPUT MODE"));
  Iris.pinModeReq(pinNumberAut, OUTPUT_PIN_MODE);

  // -- set pin on AUT to LOW
  Serial.println( F("Set pin on AUT to LOW"));
  Iris.digitalWriteReq(pinNumberAut, LOW);
  // -- check result
  delay(100);
  value = digitalRead(pinNumberMaster);
  assertEqual(value, LOW);

  // -- set pin on AUT to HIGH
  Serial.println( F("Set pin on AUT to HIGH"));
  Iris.digitalWriteReq(pinNumberAut, HIGH);
  // --check result
  delay(100);
  value = digitalRead(pinNumberMaster);
  assertEqual(value, HIGH);

  Serial.println(F("EXIT digitalWrite"));
  Serial.println("---------------------------");
  testNumber++;
  }


  test(digitRead)
  {
  byte reading[DIGITAL_READ_VALUE_MSG_SIZE];
  bool valueSet;

  Serial.println(F("ENTRY digitalRead"));

  int pinNumberMaster = 12;
  int pinNumberAut = 12; //Arduino under test

  // send DEBUG Msg
  Iris.debugMsgReq(testNumber);

  // ---- Set pin on AUT to INPUT MODE
  // do NOT set INPUT_PULLUP
  Serial.println( F("Set pin on AUT to INPUT MODE"));
  Iris.pinModeReq(pinNumberAut, INPUT_PIN_MODE);

  delay(100);

  // ---- set pinNumberMaster to High on master
  Serial.println( F("Set pin on Tester to HIGH"));
  valueSet = HIGH;
  digitalWrite(pinNumberMaster, valueSet);

  // ---- Digital_read_request
  Serial.println( F("Request digital read on AUT"));
  Iris.digitalReadReq(pinNumberAut);

  // ---- check data returned
  Serial.println( F("Check value read on AUT"));
  if (DIGITAL_READ_VALUE_MSG_SIZE <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading) it is the msgId
    assertEqual(reading[0], DIGITAL_READ_VALUE_MSG_ID);
    reading[1] = Wire.read();
    // decode digitalRead
    int pinNumber = 0;
    byte valueRead;
    IrisClass::decodeDigitalReadValue(&reading[1], &pinNumber, &valueRead);
    assertEqual(pinNumber, pinNumberAut);
    assertEqual(valueRead, valueSet);
  }

  // ------------- set pinNumberMaster to Low on master
  Serial.println( F("Set pin on Tester to HIGH"));
  valueSet = HIGH;
  digitalWrite(pinNumberMaster, valueSet);

  // ---- Digital_read_request
  Serial.println( F("Request digital read on AUT"));
  Iris.digitalReadReq(pinNumberAut);

  // ---- check data returned
  Serial.println( F("Check value read on AUT"));
  if (DIGITAL_READ_VALUE_MSG_SIZE <= Wire.available()) { // if two bytes were received
    reading[0] = Wire.read();  // receive high byte (overwrites previous reading) it is the msgId
    assertEqual(reading[0], DIGITAL_READ_VALUE_MSG_ID);
    reading[1] = Wire.read();
    // decode digitalRead
    int pinNumber = 0;
    byte valueRead;
    IrisClass::decodeDigitalReadValue(&reading[1], &pinNumber, &valueRead);
    assertEqual(pinNumber, pinNumberAut);
    assertEqual(valueRead, valueSet);
  }

  Serial.println(F("EXIT digitalRead"));
  Serial.println("---------------------------");
  testNumber++;
  }*/


void setup()
{
  testNumber = 0;
  // pinMode(A0, INPUT);
  Iris.beginMaster(I2C_SLAVE_ADDR);
  Serial.println(F("---------------------------"));
}

void loop()
{
  Test::run();
}
