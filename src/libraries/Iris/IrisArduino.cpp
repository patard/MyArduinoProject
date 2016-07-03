#include "IrisArduino.h"
#define DEFAULT_SERIAL_BAUD_RATE 9600


extern "C" {
#include <string.h>
#include <stdlib.h>
}

#define VERSION_BLINK_PIN 13

/**
* function call on interrupt genrated by I2C message reception
* @param numBytes: number of byes receives (cf arduino Wire library for more explanation)
*/
void receiveData(int numBytes) { // numBytes: the number of bytes read from the master
	int bytePosition = 0;
	char tmpBuff[CONTAINER_SIZE];
	// while data available on I2C
	while (Wire.available()) {
	   if ( bytePosition >= CONTAINER_SIZE ) {
           Wire.read(); // purge oversized messages
	   }
	   else {
	       tmpBuff[bytePosition] = Wire.read();
	   }
	   bytePosition ++;
	} // while
	if ( ! g_MsgContainerQueue.isFull() ) {
	   g_MsgContainerQueue.add( tmpBuff, bytePosition);
    }
}

void sendData() {
	if (g_OutputMsgSize)
		Wire.write(g_pOutputMsgBuf, g_OutputMsgSize);
	g_OutputMsgSize = 0;
}

//******************************************************************************
//* Constructors
//******************************************************************************
/**
* The Iris class.
* An instance named "Iris" is created automatically for the user.
*/
IrisArduinoClass::IrisArduinoClass()
{

}
//******************************************************************************
//* Public Methods
//******************************************************************************
/**
* Initialize the default Serial transport at the default baud of 57600.
* Initialize the default I2C address of the device to 0x05
*/
void IrisArduinoClass::begin(void)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = 0x05;
	this->initI2cAsSlave(_i2cAdress); // initialize i2c
	Serial.println(__func__);
    blinkVersion();
}

/**
* Initialize the I2C address of the device
* @param is the i2c address to set
*/
void IrisArduinoClass::begin(int i2cAddress)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = i2cAddress;
	this->initI2cAsSlave(_i2cAdress); // initialize i2c
	Serial.println(__func__);
    blinkVersion();
}


void IrisArduinoClass::beginMaster(int i2cAddress)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = i2cAddress;
    Wire.begin(); // join i2c bus (address optional for master)
	//Serial.println(__func__);
}


//******************************************************************************
//* Private Methods
//******************************************************************************
void IrisArduinoClass::initI2cAsSlave(int i2cAddress)
{
	Wire.begin(i2cAddress);
	// define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
	Serial.println(__func__);
}



void IrisArduinoClass::debugMsgReq(int id)
{
    Wire.beginTransmission(_i2cAdress); // transmit to device
    Wire.write(DEBUG_MSG_ID);
    Wire.write(id);
    Wire.endTransmission(); // stop transmitting
}



void IrisArduinoClass::pinModeReq(int pinNum, byte mode)
{
    byte toWrite[PIN_MODE_MSG_SIZE];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodePinMode(&toWrite[0], &nbByteToWrite, pinNum, mode); 
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::digitalWriteReq(int pinNum, byte valueToSet)
{
    byte toWrite[DIGITAL_WRITE_MSG_SIZE];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodeDigitalWrite(&toWrite[0], &nbByteToWrite, pinNum, valueToSet); 
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::digitalReadReq(int pinNum)
{
    byte toWrite[DIGITAL_READ_MSG_SIZE];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    // encode msg digitalRead on pinNumber
    encodeDigitalRead(&toWrite[0], &nbByteToWrite, pinNum);
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting

    // ---- wait for things to happen
    delay(100);

    // ---- request reading from sensor
    Wire.requestFrom(_i2cAdress, DIGITAL_READ_VALUE_MSG_SIZE);    // request 2 bytes from slave device
}


void IrisArduinoClass::analogWriteReq(int pinNum, int valueToSet)
{
    byte toWrite[ANALOG_WRITE_MSG_SIZE];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodeAnalogWrite(&toWrite[0], &nbByteToWrite, pinNum, valueToSet);
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::analogReadReq(int pinNum)
{
    byte toWrite[ANALOG_READ_MSG_SIZE];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    // encode msg digitalRead on pinNumber
    encodeAnalogRead(&toWrite[0], &nbByteToWrite, pinNum);
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting

    // ---- wait for things to happen
    delay(100);

    // ---- request reading from sensor
    Wire.requestFrom(_i2cAdress, ANALOG_READ_VALUE_MSG_SIZE);    // request 2 bytes from slave device
}

void IrisArduinoClass::printDebug(const String &functionName, const String &strToPrint)
{
	//#ifdef _DEBUG_
	Serial.print(functionName);
	Serial.print(" IrisArduino | ");
	Serial.println(strToPrint);
	//#endif
}



/**
 * Sets the name and version of the firmware. This is not the same version as the Firmata protocol
 * (although at times the firmware version and protocol version may be the same number).
 * @param name A pointer to the name char array
 * @param major The major version number
 * @param minor The minor version number
 */
/*void IrisArduinoClass::setFirmwareNameAndVersion(const char *name, byte major, byte minor)
{
  const char *firmwareName;
  const char *extension;

  // parse out ".cpp" and "applet/" that comes from using __FILE__
  extension = strstr(name, ".cpp");
  firmwareName = strrchr(name, '/');

  if (!firmwareName) {
    // windows
    firmwareName = strrchr(name, '\\');
  }
  if (!firmwareName) {
    // user passed firmware name
    firmwareName = name;
  } else {
    firmwareName ++;
  }

  if (!extension) {
    _firmwareVersionCount = strlen(firmwareName) + 2;
  } else {
    _firmwareVersionCount = extension - firmwareName + 2;
  }

  // in case anyone calls setFirmwareNameAndVersion more than once
  free(_firmwareVersionVector);

  _firmwareVersionVector = (byte *) malloc(_firmwareVersionCount + 1);
  _firmwareVersionVector[_firmwareVersionCount] = 0;
  _firmwareVersionVector[0] = major;
  _firmwareVersionVector[1] = minor;
  strncpy((char *)_firmwareVersionVector + 2, firmwareName, _firmwareVersionCount - 2);
}*/

void IrisArduinoClass::blinkVersion(void)
{
//#if defined(VERSION_BLINK_PIN)
 // if (blinkVersionDisabled) return;
  // flash the pin with the protocol version
  pinMode(VERSION_BLINK_PIN, OUTPUT);
  strobeBlinkPin(VERSION_BLINK_PIN, IRIS_FIRMWARE_MAJOR_VERSION, 250, 500);
  delay(1000);
  strobeBlinkPin(VERSION_BLINK_PIN, IRIS_FIRMWARE_MINOR_VERSION, 250, 500);
  delay(1000);
//#endif
}



/**
 * Flashing the pin for the version number
 * @private
 * @param pin The pin the LED is attached to.
 * @param count The number of times to flash the LED.
 * @param onInterval The number of milliseconds for the LED to be ON during each interval.
 * @param offInterval The number of milliseconds for the LED to be OFF during each interval.
 */
void IrisArduinoClass::strobeBlinkPin(byte pin, int count, int onInterval, int offInterval)
{
  byte i;
  for (i = 0; i < count; i++) {
    delay(offInterval);
    digitalWrite(pin, HIGH);
    delay(onInterval);
    digitalWrite(pin, LOW);
  }
}

// make one instance for the user to use
IrisArduinoClass Iris;