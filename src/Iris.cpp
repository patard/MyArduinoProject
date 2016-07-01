#include "Iris.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include "IrisBehaviour.h"
#define LOG(x) printDebug(__func__, F(x))
#else
#define LOG(x) this->printDebug(__func__, x)//TODO
#endif

// TODO add for other implementation

// make one instance for the user to use
MsgContainer g_MsgContainerQueue;
byte g_pOutputMsgBuf[10]; // buffer ecriture
int g_OutputMsgSize = 0;

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
			// PinModeMsg pinModeMsg; // PinModeMsg herite de Msg
			// pinModeMsg.set();
			treatPinModeMsg(aMsg, bufferSize);
		}
		break;
		case DIGITAL_WRITE_MSG_ID:
		{
			LOG(" => DigitalWriteMsg");
			treatDigitalWriteMsg(aMsg, bufferSize);
		}
		break;
		case DIGITAL_WRITES_MSG_ID:
		{
			LOG(" => DigitalWritesMsg");
			treatDigitalWritesMsg(aMsg, bufferSize);	
		}
		break;
		case ANALOG_WRITE_MSG_ID:
		{
			LOG(" => AnalogWriteMsg");
			treatAnalogWriteMsg(aMsg, bufferSize);	
		}
		break;
		case GET_IDL_VERSION_MSG_ID:
		{
			LOG(" => IdlVersionMsg");
			treatIdlVersionMsg(aMsg, bufferSize);		
		}
		break;
		case DIGITAL_READ_MSG_ID:
		{
			LOG(" => DigitalReadMsg");
			digitalReadMsg(aMsg, bufferSize);	
		}
		break;
		case ANALOG_READ_MSG_ID:
		{
			LOG(" => AnalogReadMsg");
			analogReadMsg(aMsg, bufferSize);	
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
void IrisClass::treatPinModeMsg(byte data[], int msgSize)
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

void IrisClass::treatDigitalWriteMsg(byte data[], int msgSize)
{
	if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
	int pinNumber = data[1] >> 2; // extract pin number from data
	byte valueToSet = data[1] & 0x1; // extract value to set from data
	
	
	_irisBehaviour->digitalWrite(pinNumber, valueToSet);
}

// write on 2 pins
void IrisClass::treatDigitalWritesMsg(byte data[], int msgSize)
{
	if ( msgSize != 3 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber1 = data[1] >> 2;
	int pinNumber2 = data[2] >> 2;
	byte valueToSet1 = data[1] & 0x1;
	byte valueToSet2 = data[2] & 0x1;
	
	_irisBehaviour->digitalWrite(pinNumber1, valueToSet1);
	_irisBehaviour->digitalWrite(pinNumber2, valueToSet2);
}

void IrisClass::treatAnalogWriteMsg(byte data[], int msgSize)
{
	if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber = data[1];
	byte valueToSet = data[2];

	_irisBehaviour->analogWrite(pinNumber, valueToSet);
}


void IrisClass::treatIdlVersionMsg(byte data[], int msgSize)
{
	if ( msgSize != 1 ) {
		LOG("Bad message size");
		return;
	}

	// encode IDL_Version msg, write in global var
	g_OutputMsgSize = 2;
	g_pOutputMsgBuf[0] = IDL_VERSION_MSG_ID;
	g_pOutputMsgBuf[1] = Iris_PROTOCOL_MAJOR_VERSION;
	g_pOutputMsgBuf[1] = ((g_pOutputMsgBuf[1] << 4) | Iris_PROTOCOL_MINOR_VERSION);
}


void IrisClass::digitalReadMsg(byte data[], int msgSize) // ??? Bizarre => j'ai changé le sens
{
	if ( msgSize != 1 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber = data[1];
	bool valueRead = _irisBehaviour->digitalRead(pinNumber);
	
	// encode msg, write in global var
	g_OutputMsgSize = 2;
	g_pOutputMsgBuf[0] = DIGITAL_READ_VALUE_MSG_ID;
	g_pOutputMsgBuf[1] = valueRead | (pinNumber << 2);
	
}


void IrisClass::analogReadMsg(byte data[], int msgSize) // ??? Bizarre => j'ai changé le sens
{
	if ( msgSize != 1 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber = data[1];
	int valueRead = _irisBehaviour->analogRead(pinNumber);
	
	// encode msg, write in global var
	g_OutputMsgSize = 3;
	g_pOutputMsgBuf[0] = ANALOG_READ_VALUE_MSG_ID;
	g_pOutputMsgBuf[1] = (valueRead >> 8)| (pinNumber << 2);
	g_pOutputMsgBuf[2] = valueRead & 0xFF;
	
}


void IrisClass::decodeDigitalRead(byte aByte, int * pinNum, int * valueRead)
{
	*pinNum = aByte & 0x3;
	*valueRead = aByte >> 2;
}

void IrisClass::printDebug(const String &functionName, const String &strToPrint)
{
	//TODO be derived
	Serial.print(functionName);
	Serial.print(" IRIS | ");
	Serial.println(strToPrint);
}
