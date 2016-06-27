#include "Isis.h"

#define LOG(x) this->printDebug(__func__, F(x))

#define UNRELEVANT_I2C_ADDR -1


volatile boolean newData = false;
int g_numBytes;


void receiveData(int numBytes) { // numBytes: the number of bytes read from the master
	int nbReadByte = 0;
    
    g_numBytes = numBytes;
	// while data available on I2C
	while (Wire.available()) {
		if ( nbReadByte >= BUFFER_SIZE_MAX ) {
			Wire.read(); // purge oversized messages
			//printDebug("Purge oversized message");
		}
		else {
			g_inputMsgBuf[nbReadByte] = Wire.read();
		}
		nbReadByte ++;
        newData = true;
	}
	// Decode message
	//decodeMessage(_inputMsgBuf, nbReadByte);*/
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
* The Isis class.
* An instance named "Isis" is created automatically for the user.
*/
IsisClass::IsisClass()
{
	_i2cAdress = UNRELEVANT_I2C_ADDR; // init to unrelevant I2C address
}

//******************************************************************************
//* Public Methods
//******************************************************************************
/**
* Initialize the default Serial transport at the default baud of 57600.
* Initialize the default I2C address of the device to 0x05
*/
void IsisClass::begin(void)
{
	begin(57600);
	_i2cAdress = 0x05;
	this->initI2cAsSlave(_i2cAdress);// initialize i2c 
}

/**
* Initialize the I2C address of the device
* @param is the i2c address to set
*/
void IsisClass::begin(int i2cAddress)
{
	begin(57600);	
	_i2cAdress = i2cAddress;
	this->initI2cAsSlave(_i2cAdress);
}

boolean IsisClass::hasNewData()
{
    return newData;
}


void IsisClass::decodeMessage()
{
	int bufferSize = g_numBytes;
    
    // copy : if an other interruption happens and write on global var,
    // the treatment is not affected
    for (int i=0; i < BUFFER_SIZE_MAX; i++)
    {  
        _inputMsgBuf[i] = g_inputMsgBuf[i];
    }
   
    
    switch (_inputMsgBuf[0]) // byte 0 contains the message identifier
	{
		/*case DEFINE_ENCODER_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => DefineEncoderMsg");
		#endif
		defineEncoderMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}
		case GET_ENCODER_COUNTER_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => GetEncoderCounterMsg");
		#endif
		getEncoderCounterMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}*/
		case PIN_MODE_MSG_ID:
		{
		LOG(" => PinModeMsg");
		interpretPinModeMsg(_inputMsgBuf, bufferSize);
		break;
		}
		case DIGITAL_WRITE_MSG_ID:
		{
		LOG(" => DigitalWriteMsg");
		digitalWriteMsg_received(_inputMsgBuf, bufferSize);
		break;
		}
		/*case DIGITAL_WRITES_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => DigitalWritesMsg");
		#endif
		digitalWritesMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}
		case ANALOG_WRITE_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => AnalogWriteMsg");
		#endif
		analogWriteMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}
		case GET_IDL_VERSION_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => GetIdlVersionMsg");
		#endif
		getIdlVersionMsg_received();
		break;
		}
		case GET_SOFT_VERSION_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => GetSoftVersionMsg");
		#endif
		getSoftVersionMsg_received();
		break;
		}
		case GET_STATUS_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => GetStatusMsg");
		#endif
		getStatusMsg_received();
		break;
		}
		case GET_TYPE_ARDUINO_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => GetArduinoTypeMsg");
		#endif
		getTypeArduinoMsg_received();
		break;
		}
		case DIGITAL_READ_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => digitaleReadMsg_received");
		#endif
		digitaleReadMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}
		case ANALOG_READ_MSG_ID:
		{
		#ifdef _DEBUG_
		Serial.println(" => analogReadMsg_received");
		#endif
		analogReadMsg_received(p_pMsgBuffer, p_BufferSize);
		break;
		}*/
		default :
		{
		Serial.println(" .. UNKNOWN ID !");
		break;
		}
	}
    
    // release
    newData = false;
}



//******************************************************************************
//* Private Methods
//******************************************************************************
void IsisClass::initI2cAsSlave(int i2cAddress)
{
	Wire.begin(i2cAddress);
	// define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
}


void IsisClass::interpretPinModeMsg(byte data[], int msgSize)
{
	int pinNumber = data[1] >> 2; 		// extract pin number from data
	byte pinModeToSet = data[1] & 0x3;	// extract pin mode from data
	byte arduinoPinMode = OUTPUT ;	// default initialization
	
	if ( msgSize != 2 ) {
		LOG("Bad message size");
	}
	
	switch ( pinModeToSet) // map Isis protocol value to Arduino
	{
		case INPUT_PIN_MODE: 	arduinoPinMode = INPUT; 		break;
		case OUTPUT_PIN_MODE: 	arduinoPinMode = OUTPUT;		break;
		case PULLUP_PIN_MODE:	arduinoPinMode = INPUT_PULLUP;	break;
		default:
			// impossible
		break;
	}
	// call Arduino core
	pinMode(pinNumber, arduinoPinMode);
}


void IsisClass::digitalWriteMsg_received(byte data[], int msgSize)
{
	int pinNumber = data[1] >> 2; 	// extract pin number from data
	byte valueToSet = data[1] & 0x1; // extract value to set from data
	byte arduinoValue = HIGH; // default initialization
	
	if ( msgSize != 1 ) {
		LOG("Bad message size");		
	}
	
	
	if (valueToSet == DIGITAL_LOW) {
		arduinoValue = LOW;
	}
	// call Arduino core
	digitalWrite(pinNumber, arduinoValue);
}

void IsisClass::printDebug(const String &functionName, const String &strToPrint) 
{
//#ifdef _DEBUG_
	Serial.print(functionName);
	Serial.print(" | ");
	Serial.println(strToPrint);
//#endif
}


// make one instance for the user to use
IsisClass Isis;