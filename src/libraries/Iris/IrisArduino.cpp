#include "IrisArduino.h"
#define DEFAULT_SERIAL_BAUD_RATE 9600

/*
* Receive I2C data
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
}


void IrisArduinoClass::beginMaster(int i2cAddress)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = i2cAddress;
	//this->initI2cAsSlave(_i2cAdress); // initialize i2c
    Wire.begin(); // join i2c bus (address optional for master)
	Serial.println(__func__);
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
    byte toWrite[2];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodePinMode(&toWrite[0], &nbByteToWrite, pinNum, mode); 
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::digitalWriteReq(int pinNum, byte valueToSet)
{
    byte toWrite[2];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodeDigitalWrite(&toWrite[0], &nbByteToWrite, pinNum, valueToSet); 
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::digitalReadReq(int pinNum)
{
    byte toWrite[2];
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
    byte toWrite[2];
    int nbByteToWrite;
    
    Wire.beginTransmission(_i2cAdress); // transmit to device
    encodeAnalogWrite(&toWrite[0], &nbByteToWrite, pinNum, valueToSet);
    Wire.write(toWrite, nbByteToWrite);
    Wire.endTransmission(); // stop transmitting
}


void IrisArduinoClass::analogReadReq(int pinNum)
{
    byte toWrite[2];
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

// make one instance for the user to use
IrisArduinoClass Iris;