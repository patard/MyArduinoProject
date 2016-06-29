#include "Iris.h"

#define LOG(x) this->printDebug(__func__, F(x))

#define DEFAULT_SERIAL_BAUD_RATE 9600
#define UNRELEVANT_I2C_ADDR -1


//******************************************************************************
//* Constructors
//******************************************************************************
I2cMsgContainer::I2cMsgContainer ()
{
	_freeIdx = 0;
	_workIdx = 0;
}

//******************************************************************************
//* Public Methods
//******************************************************************************
boolean I2cMsgContainer::isEmpty()
{
	return ( updateWorkIdx() < 0); // if no valid working index, the container is empty
}

boolean I2cMsgContainer::isFull()
{
	boolean ret = true;
	if ( this->getFreeIdx() >= 0 ) {
		ret = false; // at least one element is free
	}
	
	return ret; 
}

int I2cMsgContainer::getFreeIdx()
{
	int idx = 0;
	for (int i=0; i < MSG_NUMBER_POOL; i++) {
		idx = (_freeIdx + i) % MSG_NUMBER_POOL;
		
		if ( _container[idx].free ) {
			_freeIdx = idx;
			return idx; // at least one element is free
		}
	}
	
	Serial.println("No more free space in container");
	return -1; // no element free found, the container is full
}

int I2cMsgContainer::updateWorkIdx()
{
	//for debug : print how many used elts
            int nbUsed = 0;
            for (int j=0; j < MSG_NUMBER_POOL; j++) {
                if ( ! _container[j].free) {
                    nbUsed++;
                }
            }
    if (nbUsed > 0) {
            Serial.print(" nbElt used : ");
            Serial.println(nbUsed);
    }
            // end debug
    
    
    int idx = 0;
	for (int i=0; i < MSG_NUMBER_POOL; i++) {
		idx = (_workIdx + i) % MSG_NUMBER_POOL;
		
		if ( ! _container[idx].free ) {
			_workIdx = idx;
        
			return idx; // at least one element is NOT free
		}
	}
	
	//Serial.println("No more free working in container");
	return -1; // all element are free found
}

/*void I2cMsgContainer::addByte(int idx, int bytePosition, char c)
{
	_container[idx].data[bytePosition] = c;
	_container[idx].free = false;
	updateWorkIdx(); // will update _workIdx if necessary
}*/

void I2cMsgContainer::addMsg(int idx,  char * c, int size)
{
	for (int i = 0; i < size; i++) {
		_container[idx].data[i] = c[i];
	}
	_container[idx].nbBytes = size;
	_container[idx].free = false;
	updateWorkIdx(); // will update _workIdx if necessary
}

T_S_MsgContainer * I2cMsgContainer::getWorkMsg() 
{
	if ( _workIdx >= 0 ) {
		return &(_container[_workIdx]);
	}
	else {
		return NULL;
	}
}

void I2cMsgContainer::releaseWorkMsg()
{
	_container[_workIdx].free = true;
	updateWorkIdx(); // will update _workIdx
}


// make one instance for the user to use
I2cMsgContainer g_MsgContainer;


/*
* Receive I2C data
*/
void receiveData(int numBytes) { // numBytes: the number of bytes read from the master
	int bytePosition = 0;
	char tmpBuff[BUFFER_SIZE_MAX];
	//g_numBytes = numBytes;
	// while data available on I2C
	while (Wire.available()) {
		if ( bytePosition >= BUFFER_SIZE_MAX ) {
				Wire.read(); // purge oversized messages
				//printDebug("Purge oversized message");
		}
		else {
			tmpBuff[bytePosition] = Wire.read();
		}
		bytePosition ++;
	} // while
	
	
	if ( ! g_MsgContainer.isFull() ) {
		int idx = g_MsgContainer.getFreeIdx();			
		g_MsgContainer.addMsg(idx, tmpBuff, numBytes);		
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
	_ptMsgContainer = &g_MsgContainer;
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

boolean IrisClass::newDataAvailable()
{
	return !_ptMsgContainer->isEmpty();
}

void IrisClass::decodeMessage()
{
	//LOG("Entry");
	//int bufferSize = g_numBytes;
	int bufferSize = _ptMsgContainer->getWorkMsg()->nbBytes;
	// copy : if an other interruption happens and write on global var,
	// the treatment is not affected
	for (int i=0; i < BUFFER_SIZE_MAX; i++)
	{
		//_msg[i] = g_inputMsgBuf[i];
		_msg[i] = _ptMsgContainer->getWorkMsg()->data[i];
	}
	
	switch (_msg[0]) // byte 0 contains the message identifier
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
		interpretPinModeMsg(_msg, bufferSize);
		}
		break;
		case DIGITAL_WRITE_MSG_ID:
		{
		LOG(" => DigitalWriteMsg");
		digitalWriteMsg_received(_msg, bufferSize);
		}
		break;
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
		}
		break;
	}
	// release
	_ptMsgContainer->releaseWorkMsg();
	//g_inputMsgBuf[nextFree].free = false;
	
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