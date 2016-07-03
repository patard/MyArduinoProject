#include "Iris.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega2560__)
#warning "TODO change to IrisArduinoBehaviour"
#include "IrisBehaviour.h"                  // TODO change with IrisArduinoBehaviour
#define LOG(x) printDebug(__func__, F(x))
#else
#define LOG(x) this->printDebug(__func__, x)//TODO
#endif



MsgContainer    g_MsgContainerQueue;
byte            g_pOutputMsgBuf[ANSWER_MAX_SIZE]; 
int             g_OutputMsgSize = 0;



//******************************************************************************
//* Encode / decode msg 
//******************************************************************************
void encodePinMode(byte output[], int * size, int pinNum, byte mode)
{
    output[0] = PIN_MODE_MSG_ID; 
    output[1] = (pinNum << 2) |  mode; 
    *size = PIN_MODE_MSG_SIZE;
}


int decodePinMode(byte input[], int * pinNum, byte * mode)
{
    int ret = 0;
    
    if ( input[0] != PIN_MODE_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {
        *pinNum = input[1] >> 2; 
        *mode = input[1] & 0x3;   
    }   
    return ret;
}


void encodeDigitalWrite(byte output[], int * size,int pinNum, byte valueToSet)
{
    output[0] = DIGITAL_WRITE_MSG_ID;
    output[1] = (pinNum << 2) |  valueToSet; // NO control done on valueToSet
    *size = DIGITAL_WRITE_MSG_SIZE;
}

int decodeDigitalWrite(byte input[], int * pinNum, byte * valueToSet)
{
    int ret = 0;
    
    if ( input[0] != DIGITAL_WRITE_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {
        *pinNum = input[1] >> 2; // extract pin number from data
	   *valueToSet = input[1] & 0x1; // extract value to set from data
    }
    return ret;
}


void encodeAnalogWrite(byte output[], int * size, int pinNum, int valueToSet)
{
    // valueToSet must be [0, 255]
    output[0] = ANALOG_WRITE_MSG_ID;
    output[1] = pinNum;
    output[2] = valueToSet;
    *size = ANALOG_WRITE_MSG_SIZE;
}

int decodeAnalogWrite(byte input[],int * pinNum, byte * valueToSet)
{
    int ret = 0;
    
    if ( input[0] != ANALOG_WRITE_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {    
       *pinNum = input[1]; // extract pin number from data
	   *valueToSet = input[2]; // extract value to set from data   
    }
    return ret;
}


void encodeDigitalRead(byte  output[], int * size, int  pinNum)
{
    output[0] = DIGITAL_READ_MSG_ID;
    output[1] = pinNum;
    *size = DIGITAL_READ_MSG_SIZE;
}


int decodeDigitalRead(byte input[], int * pinNum)
{
	int ret = 0;
    
    if ( input[0] != DIGITAL_READ_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {  
        *pinNum =  input[1];
    }
    return ret;
}

void encodeAnalogRead(byte  output[], int * size, int  pinNum)
{
    output[0] = ANALOG_READ_MSG_ID;
    output[1] = pinNum;
    *size = ANALOG_READ_MSG_SIZE;
}

int decodeAnalogRead(byte input[], int * pinNum)
{
    int ret = 0;
    
    if ( input[0] != ANALOG_READ_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else { 
        *pinNum =  input[1];
    }
    return ret;
}



// value

void encodeDigitalReadValue(byte  output[], int * size, int pinNum, int valueRead) // TODO demander à Patrice de verif
{
    output[0] = DIGITAL_READ_VALUE_MSG_ID;
    output[1] = valueRead | (pinNum << 2);
    *size = DIGITAL_READ_VALUE_MSG_SIZE;
}

int decodeDigitalReadValue(byte input[], int * pinNum, byte * valueRead)
{
    int ret = 0;
    
    if ( input[0] != DIGITAL_READ_VALUE_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {
        *pinNum = input[1] >> 2; 
        *valueRead = input[1] & 0x3;   
    }
    return ret;
}


void encodeAnalogReadValue(byte output[], int * size, int pinNum, int valueRead)
{
    output[0] = ANALOG_READ_VALUE_MSG_ID;
    output[1] = (valueRead >> 8) | (pinNum << 2);
    output[2] = valueRead & 0xFF;
    *size = ANALOG_READ_VALUE_MSG_SIZE;
}

int decodeAnalogReadValue(byte input[], int * pinNum, int * valueRead)
{
   int ret = 0;
    
    if ( input[0] != ANALOG_READ_VALUE_MSG_ID ) { // bad msgID
		ret = -1;
	}
    else {
        *pinNum = input[1] >> 2; 
        *valueRead = (input[1] & 0x3 )* 0x100 + input[2];
    }
    return ret;
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
	_ptMsgContainerQueue = &g_MsgContainerQueue; // pointer to received msg queue
	_irisBehaviour = &IrisBehav;   // adapt behaviour
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
			treatDigitalReadMsg(aMsg, bufferSize);	
		}
		break;
		case ANALOG_READ_MSG_ID:
		{
			LOG(" => AnalogReadMsg");
			treatAnalogReadMsg(aMsg, bufferSize);	
		}
		break;
        case DEBUG_MSG_ID:
		{
            // only used for debug
			if (aMsg[1] == 0)  { // if testNumber is 0 this is the start of testSuite
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
	if ( msgSize != PIN_MODE_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
    
    int pinNumber;
    byte pinModeToSet;
    // extract pinNumber and pinMode from data
    if ( decodePinMode(&data[0], &pinNumber, &pinModeToSet) < 0 ) { 
		return; // pb during decode
    }
	
	// call setPinMode
	_irisBehaviour->setPinMode(pinNumber, pinModeToSet);
}

void IrisClass::treatDigitalWriteMsg(byte data[], int msgSize)
{
    if ( msgSize != DIGITAL_WRITE_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
    
    int pinNumber;
    byte valueToSet;
    // extract pinNumber and valueToSet from data
    if ( decodeDigitalWrite(&data[0], &pinNumber, &valueToSet) < 0 ) {    
		return; // pb during decode
    }
    	
	_irisBehaviour->digitalWrite(pinNumber, valueToSet);
}

// write on 2 pins
void IrisClass::treatDigitalWritesMsg(byte data[], int msgSize)
{
	if ( msgSize != DIGITAL_WRITES_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
	
    int pinNumber1, pinNumber2;
    byte valueToSet1, valueToSet2;
    byte tmp[DIGITAL_WRITE_MSG_SIZE];
    // extract pinNumber and valueToSet from data
    tmp[0] = data[0];
    tmp[1] = data[1];
    if ( decodeDigitalWrite(tmp, &pinNumber1, &valueToSet1) < 0 ) {
         return; // pb during decode
    }
    tmp[1] = data[2];
    if ( decodeDigitalWrite(tmp, &pinNumber2, &valueToSet2) < 0 ) {
        return; // pb during decode
    }
	
	_irisBehaviour->digitalWrite(pinNumber1, valueToSet1);
	_irisBehaviour->digitalWrite(pinNumber2, valueToSet2);
}

void IrisClass::treatAnalogWriteMsg(byte data[], int msgSize)
{
	if ( msgSize != ANALOG_WRITE_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber;
	byte valueToSet;
    // extract pinNumber and valueToSet from data
    if (  decodeAnalogWrite(&data[0], &pinNumber, &valueToSet) < 0 ) {
        return; // pb during decode
    }

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
	g_pOutputMsgBuf[1] = IRIS_PROTOCOL_MAJOR_VERSION;
	g_pOutputMsgBuf[1] = ((g_pOutputMsgBuf[1] << 4) | IRIS_PROTOCOL_MINOR_VERSION);
}


void IrisClass::treatDigitalReadMsg(byte data[], int msgSize) // ??? Bizarre => j'ai changé le sens
{
	if ( msgSize != DIGITAL_READ_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
	  
    int pinNumber;
    // extract pinNumber from data
    if ( decodeDigitalRead(&data[0], &pinNumber) < 0 ) {
         return; // pb during decode
    }
    
	bool valueRead = _irisBehaviour->digitalRead(pinNumber);

	// encode msg, write in global var : it will be ready for reading    
    encodeDigitalReadValue(&g_pOutputMsgBuf[0], &g_OutputMsgSize, pinNumber, valueRead);	
}


void IrisClass::treatAnalogReadMsg(byte data[], int msgSize) // ??? Bizarre => j'ai changé le sens
{
	if ( msgSize != ANALOG_READ_MSG_SIZE ) {
		LOG("Bad message size");
		return;
	}
	
	int pinNumber;
    // extract pinNumber from data
    if ( decodeAnalogRead(&data[0], &pinNumber) < 0 ) {
        return; // pb during decode
    }
    
	int valueRead = _irisBehaviour->analogRead(pinNumber);
	
	// encode msg, write in global var    
    encodeAnalogReadValue(&g_pOutputMsgBuf[0], &g_OutputMsgSize, pinNumber, valueRead);	
}


//******************************************************************************
//* request 
//******************************************************************************
void IrisClass::debugMsgReq(int id)
{
    //To implement in derived class
}

void IrisClass::pinModeReq(int pinMode, byte mode)
{
    //To implement in derived class
}

void IrisClass::digitalWriteReq(int pinMode, byte valueToSet)
{
    //To implement in derived class
}

void IrisClass::analogWriteReq(int pinMode, int valueToSet)
{
    //To implement in derived class
}

void IrisClass::digitalReadReq(int pinMode)
{
   //To implement in derived class
}

void IrisClass::analogReadReq(int pinMode)
{
    //To implement
}





void IrisClass::printDebug(const String &functionName, const String &strToPrint)
{
	//TODO be derived
	Serial.print(functionName);
	Serial.print(" IRIS | ");
	Serial.println(strToPrint);
}