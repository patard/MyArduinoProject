#include "Iris.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include "IrisBehaviour.h"
#define LOG(x) printDebug(__func__, F(x))
#else
#define LOG(x) this->printDebug(__func__, x)//TODO
#endif 

// TODO add for other implementation



// make one instance for the user to use
//MsgContainer g_MsgContainerQueue;



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

boolean IrisClass::available()
{
	return !_ptMsgContainerQueue->isEmpty();
}

void IrisClass::decodeMessage()
{
	byte * aMsg = _ptMsgContainerQueue->currentContainerContent(); // the message that is processed
	int bufferSize = _ptMsgContainerQueue->currentContainerSize();
	
	switch (aMsg[0]) // byte 0 contains the message identifier
	{
		case PIN_MODE_MSG_ID:
		{
            LOG(" => PinModeMsg");
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
			LOG(" .. UNKNOWN ID !");
		}
		break;
	}
	// release
	_ptMsgContainerQueue->freeCurrentContainer();

}
//******************************************************************************
//* Private Methods
//******************************************************************************

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
	//TODO
    Serial.print(functionName);
	Serial.print(" IRIS | ");
	Serial.println(strToPrint);
}

// make one instance for the user to use
//IrisClass Iris;