#include "Iris.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega2560__)
#include "IrisBehaviour.h" // TODO change with IrisArduinoBehaviour
#define LOG(x) printDebug(__func__, F(x))
#else
#define LOG(x) this->printDebug(__func__, x)//TODO
#endif


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
        case DEBUG_MSG_ID:
		{
			if (aMsg[1] == 0)  {
                LOG("  "); LOG("  ");
                LOG(" ===== START TEST SUITE ===== ");	
            }
            else {
                LOG(" ===== DEBUG ===== ");	
            }
            
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
    
    int pinNumber;
    byte pinModeToSet;
    // extract pinNumber and pinMode from data
    this->decodePinMode(&data[1], &pinNumber, &pinModeToSet);
	
	// call setPinMode
	_irisBehaviour->setPinMode(pinNumber, pinModeToSet);
}

void IrisClass::treatDigitalWriteMsg(byte data[], int msgSize)
{
    if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
    
    int pinNumber;
    byte valueToSet;
    // extract pinNumber and valueToSet from data
    this->decodeDigitalWrite(&data[1], &pinNumber, &valueToSet);
    	
	_irisBehaviour->digitalWrite(pinNumber, valueToSet);
}

// write on 2 pins
void IrisClass::treatDigitalWritesMsg(byte data[], int msgSize)
{
	if ( msgSize != 3 ) {
		LOG("Bad message size");
		return;
	}
	
    int pinNumber1, pinNumber2;
    byte valueToSet1, valueToSet2;
    // extract pinNumber and valueToSet from data
    this->decodeDigitalWrite(&data[1], &pinNumber1, &valueToSet1);
    this->decodeDigitalWrite(&data[2], &pinNumber2, &valueToSet2);
	
	_irisBehaviour->digitalWrite(pinNumber1, valueToSet1);
	_irisBehaviour->digitalWrite(pinNumber2, valueToSet2);
}

void IrisClass::treatAnalogWriteMsg(byte data[], int msgSize)
{
	if ( msgSize != 3 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber;
	byte valueToSet;
    // extract pinNumber and valueToSet from data
    decodeAnalogWrite(&data[1], &pinNumber, &valueToSet);

	_irisBehaviour->analogWrite(pinNumber, valueToSet);
}


void IrisClass::treatIdlVersionMsg(byte data[], int msgSize) // TODO
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
	if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
	  
    int pinNumber;
    // extract pinNumber from data
    decodeDigitalRead(&data[1], &pinNumber);
    
	bool valueRead = _irisBehaviour->digital_Read(pinNumber);

	// encode msg, write in global var : it will be ready for reading
	g_OutputMsgSize = 2;
	g_pOutputMsgBuf[0] = DIGITAL_READ_VALUE_MSG_ID;
        
    encodeDigitalReadValue(&g_pOutputMsgBuf[1], pinNumber, valueRead);
   // Serial.println(__func__);
   // Serial.println(g_pOutputMsgBuf[1]);    	
}


void IrisClass::analogReadMsg(byte data[], int msgSize) // ??? Bizarre => j'ai changé le sens
{
	if ( msgSize != 2 ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber;
    // extract pinNumber from data
    decodeAnalogRead(&data[1], &pinNumber);
    
	int valueRead = _irisBehaviour->analogRead(pinNumber);
	
	// encode msg, write in global var
	g_OutputMsgSize = 3;
	g_pOutputMsgBuf[0] = ANALOG_READ_VALUE_MSG_ID;
    
    encodeAnalogReadValue(&g_pOutputMsgBuf[1], pinNumber, valueRead);	
}


//******************************************************************************
//* request 
//******************************************************************************
void IrisClass::debugMsgReq(int id)
{
    //To implement
}


void IrisClass::pinModeReq(int pinMode, byte mode)
{
    //To implement
}

void IrisClass::encodePinMode(byte output[], int * size, int pinNum, byte mode)
{
    output[0] = PIN_MODE_MSG_ID; 
    output[1] = (pinNum << 2) |  mode; 
    *size = 2;
}


void IrisClass::decodePinMode(byte input[], int * pinNum, byte * mode)
{
	*pinNum = input[0] >> 2; 
    *mode = input[0] & 0x3;   
}


void IrisClass::digitalWriteReq(int pinMode, byte valueToSet)
{
     //To implement
}

void IrisClass::encodeDigitalWrite(byte output[], int * size,int pinNum, byte valueToSet)
{
    output[0] = DIGITAL_WRITE_MSG_ID;
    output[1] = (pinNum << 2) |  valueToSet;
    *size = 2;
}

void IrisClass::decodeDigitalWrite(byte input[], int * pinNum, byte * valueToSet)
{
    *pinNum = input[0] >> 2; // extract pin number from data
	*valueToSet = input[0] & 0x1; // extract value to set from data    
}


void IrisClass::analogWriteReq(int pinMode, int valueToSet)
{
    //To implement
}

void IrisClass::encodeAnalogWrite(byte output[], int * size, int pinNum, int valueToSet)
{
    // valueToSet must be [0, 255]
    output[0] = ANALOG_WRITE_MSG_ID;
    output[1] = pinNum;
    output[2] = valueToSet;
    *size = 3;
}

void IrisClass::decodeAnalogWrite(byte input[],int * pinNum, byte * valueToSet)
{
    *pinNum = input[0]; // extract pin number from data
	*valueToSet = input[1]; // extract value to set from data   
}

void IrisClass::digitalReadReq(int pinMode)
{
    //To implement
}

void IrisClass::encodeDigitalRead(byte  output[], int * size, int  pinNum)
{
    output[0] = DIGITAL_READ_MSG_ID;
    output[1] = pinNum;
    *size = 2;
}


void IrisClass::decodeDigitalRead(byte input[], int * pinNum)
{
	*pinNum =  input[0];
}


void IrisClass::analogReadReq(int pinMode)
{
    //To implement
}

void IrisClass::decodeAnalogRead(byte input[], int * pinNum)
{
    *pinNum =  input[0];
}

void IrisClass::encodeAnalogRead(byte  output[], int * size, int  pinNum)
{
    output[0] = ANALOG_READ_MSG_ID;
    output[1] = pinNum;
    *size = 2;
}


//******************************************************************************
//* value
//******************************************************************************
void IrisClass::encodeDigitalReadValue(byte  output[], int pinNum, int valueRead) // TODO demander à Patrice de verif
{
    //Serial.println(__func__);
    //Serial.println(valueRead | (pinNum << 2));
    output[0] = valueRead | (pinNum << 2);
}

void IrisClass::decodeDigitalReadValue(byte input[], int * pinNum, byte * valueRead)
{
    *pinNum = input[0] >> 2; 
    *valueRead = input[0] & 0x3;   
  //  Serial.print(__func__);
  //  Serial.println(*valueRead);
}


void IrisClass::encodeAnalogReadValue(byte output[], int pinNum, int valueRead) // TODO demander à Patrice de verif
{
    //Serial.println(__func__);
    //Serial.println(valueRead | (pinNum << 2));
    output[0] = 0;
    output[0] = (valueRead >> 8) | (pinNum << 2);
    output[1] = valueRead & 0xFF;
}

void IrisClass::decodeAnalogReadValue(byte input[], int * pinNum, int * valueRead)
{
    *pinNum = input[0] >> 2; 
    *valueRead = (input[0] & 0x3 )* 0x100 + input[1];   
}


void IrisClass::printDebug(const String &functionName, const String &strToPrint)
{
	//TODO be derived
	Serial.print(functionName);
	Serial.print(" IRIS | ");
	Serial.println(strToPrint);
}