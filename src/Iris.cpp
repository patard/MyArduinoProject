#include "Iris.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
//#include "IrisArduino.h"
#include "IrisBehaviour.h"
#endif 

// TODO add for other implementation


#define LOG(x) this->printDebug(__func__, F(x))
#define DEFAULT_SERIAL_BAUD_RATE 9600
#define UNRELEVANT_I2C_ADDR -1

// make one instance for the user to use
FifoContainer g_MsgContainerQueue;

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
	/*if (g_OutputMsgSize)
	Wire.write(g_pOutputMsgBuf, g_OutputMsgSize);
	g_OutputMsgSize = 0;*/
}

//******************************************************************************
//* Constructors
//******************************************************************************
/**
* The Iris class.
* An instance named "Iris" is created automatically for the user.
*/
IrisClass::IrisClass()
{
	_i2cAdress = UNRELEVANT_I2C_ADDR; // init to unrelevant I2C address
	_ptMsgContainerQueue = &g_MsgContainerQueue;
	
	_irisBehaviour = &IrisBehav;
}
//******************************************************************************
//* Public Methods
//******************************************************************************
/**
* Initialize the default Serial transport at the default baud of 57600.
* Initialize the default I2C address of the device to 0x05
*/
void IrisClass::begin(void)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = 0x05;
	this->initI2cAsSlave(_i2cAdress); // initialize i2c
}
/**
* Initialize the I2C address of the device
* @param is the i2c address to set
*/
void IrisClass::begin(int i2cAddress)
{
	Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
	_i2cAdress = i2cAddress;
	this->initI2cAsSlave(_i2cAdress); // initialize i2c
}

boolean IrisClass::available()
{
	return !_ptMsgContainerQueue->isEmpty();
}

void IrisClass::decodeMessage()
{
	byte * aMsg = _ptMsgContainerQueue->currentContainerContent(); // the message that is processed
	int bufferSize = _ptMsgContainerQueue->currentContainerSize();
	// copy : if an other interruption happens and write on global var,
	// the treatment is not affected
	/*for (int i=0; i < CONTAINER_SIZE; i++)
	{
		_msg[i] = _ptMsgContainerQueue->currentContainerContent()[i];
	}*/
	
	switch (aMsg[0]) // byte 0 contains the message identifier
	{
		case PIN_MODE_MSG_ID:
		{
		LOG(" => PinModeMsg");
		interpretPinModeMsg(aMsg, bufferSize);
			decodePinModeMsg(aMsg, bufferSize);
		}
		break;
		case DIGITAL_WRITE_MSG_ID:
		{
		LOG(" => DigitalWriteMsg");
		digitalWriteMsg_received(aMsg, bufferSize);
		}
		break;
	
		default :
		{
			Serial.println(" .. UNKNOWN ID !");
		}
		break;
	}
	// release
	_ptMsgContainerQueue->freeCurrentContainer();

}
//******************************************************************************
//* Private Methods
//******************************************************************************
void IrisClass::initI2cAsSlave(int i2cAddress)
{
	Wire.begin(i2cAddress);
	// define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
}

void IrisClass::decodePinModeMsg(byte data[], int msgSize)
{	
	if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber = data[1] >> 2; // extract pin number from data
	byte pinModeToSet = data[1] & 0x3; // extract pin mode from data

	// call setPinMode
	_irisBehaviour->setPinMode(pinNumber, pinModeToSet);
}


void IrisClass::interpretPinModeMsg(byte data[], int msgSize)
{
	int pinNumber = data[1] >> 2; // extract pin number from data
	byte pinModeToSet = data[1] & 0x3; // extract pin mode from data
	byte arduinoPinMode = OUTPUT ; // default initialization
	if ( msgSize != 2 ) {
		LOG("Bad message size");
	}
	switch ( pinModeToSet) // map Iris protocol value to Arduino
	{
		case INPUT_PIN_MODE: arduinoPinMode = INPUT; break;
		case OUTPUT_PIN_MODE: arduinoPinMode = OUTPUT; break;
		case PULLUP_PIN_MODE: arduinoPinMode = INPUT_PULLUP; break;
		default:
		// impossible
		break;
	}
	// call Arduino core
	pinMode(pinNumber, arduinoPinMode);
}

void IrisClass::digitalWriteMsg_received(byte data[], int msgSize)
{
	int pinNumber = data[1] >> 2; // extract pin number from data
	byte valueToSet = data[1] & 0x1; // extract value to set from data
	byte arduinoValue = HIGH; // default initialization
	if ( msgSize != 2 ) {
		LOG("Bad message size");
	}
	if (valueToSet == DIGITAL_LOW) {
		arduinoValue = LOW;
	}
	// call Arduino core
	digitalWrite(pinNumber, arduinoValue);
}

void IrisClass::printDebug(const String &functionName, const String &strToPrint)
{
	//#ifdef _DEBUG_
	Serial.print(functionName);
	Serial.print(" | ");
	Serial.println(strToPrint);
	//#endif
}

// make one instance for the user to use
IrisClass Iris;