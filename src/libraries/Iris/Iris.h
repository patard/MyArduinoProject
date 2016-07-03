/*
Iris.h - Iris library v0.1 - 2016-6-26
Copyright (c) 2016 Thales Services. All rights reserved.

Iris is a communication protocol using I2C.
For now, the only implementation is the one in which an Arduino UNO or MEGA is the slave.
But it can be extended.

TODO : give the internet page link
*/

#ifndef Iris_h
#define Iris_h

// define only Arduino MEGA and UNO
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include <Arduino.h> // Arduino core
#include <Wire.h> // I2C library
#else
#error "Not implemented"
//TODO define byte
#endif

#include "MsgContainer.h"
#include "IrisBehaviour.h"

/* Version numbers for the protocol. The protocol is still changing, so these
* version numbers are important.
* Query using the REPORT_VERSION message.
*/
#define IRIS_PROTOCOL_MAJOR_VERSION 0
#define IRIS_PROTOCOL_MINOR_VERSION 6

/* Version numbers for the Iris library.
* The firmware version will not always equal the protocol version going forward.
* Query using the REPORT_FIRMWARE message.
*/
#define IRIS_FIRMWARE_MAJOR_VERSION 0
#define IRIS_FIRMWARE_MINOR_VERSION 1

// message command bytes
#define PIN_MODE_MSG_ID             0x01
#define PIN_MODE_MSG_SIZE           2
#define DIGITAL_READ_MSG_ID         0x02
#define DIGITAL_READ_MSG_SIZE       2
#define DIGITAL_WRITE_MSG_ID        0x03
#define DIGITAL_WRITE_MSG_SIZE      2
#define DIGITAL_WRITES_MSG_ID       0x04
#define DIGITAL_WRITES_MSG_SIZE     3
#define ANALOG_READ_MSG_ID          0x05
#define ANALOG_READ_MSG_SIZE        2
#define ANALOG_WRITE_MSG_ID         0x06
#define ANALOG_WRITE_MSG_SIZE       3
#define DEFINE_ENCODER_MSG_ID       0x07
#define GET_ENCODER_COUNTER_MSG_ID  0x08
#define GET_SOFT_VERSION_MSG_ID     0x09
#define GET_STATUS_MSG_ID           0x0A
#define GET_TYPE_ARDUINO_MSG_ID     0x0B
#define GET_IDL_VERSION_MSG_ID      0x0C

#define DEBUG_MSG_ID                0xFF

#define DIGITAL_READ_VALUE_MSG_ID   0xF2
#define DIGITAL_READ_VALUE_MSG_SIZE 2
#define ANALOG_READ_VALUE_MSG_ID    0xF5
#define ANALOG_READ_VALUE_MSG_SIZE  3
#define ENCODER_COUNTER_MSG_ID      0xF8
#define STATUS_MSG_ID               0xFA
#define TYPE_ARDUINO_MSG_ID         0xFB
#define IDL_VERSION_MSG_ID          0xFC
#define FIRM_VERSION_MSG_ID         0xF9


#define ANSWER_MAX_SIZE             5
#define DIGITAL_READ_VALUE_MSG_SIZE 2
#define ANALOG_READ_VALUE_MSG_SIZE  3

// Digital pin mode (Iris protocol value)
#define INPUT_PIN_MODE 0
#define OUTPUT_PIN_MODE 1
#define PULLUP_PIN_MODE 2

// Digital pin value (Iris protocol value)
#define DIGITAL_LOW 0
#define DIGITAL_HIGH 1
#define UNRELEVANT_I2C_ADDR -1

// encode / decode messages : correspond to the Patrice's enICCma 

/**
* decode pin mode message
* @param input: message to decode
* @param pinNum: pin number decoded
* @param mode: mode decoded (must be INPUT_PIN_MODE, OUTPUT_PIN_MODE or PULLUP_PIN_MODE)
* @return -1 if a pb occured, 0 otherwise
*/
int decodePinMode(byte input[], int * pinNum, byte * mode);

/**
* encode pin mode message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
* @param mode: mode (must be INPUT_PIN_MODE, OUTPUT_PIN_MODE or PULLUP_PIN_MODE)
*/
void encodePinMode(byte output[], int * size,int pinNum, byte mode);

/**
* decode digital write message
* @param input: message to decode
* @param pinNum: pin number decoded
* @param valueToSet: digital value to set (must be DIGITAL_LOW or DIGITAL_HIGH)
* @return -1 if a pb occured, 0 otherwise
*/
int decodeDigitalWrite(byte input[], int * pinNum, byte * valueToSet);

/**
* encode digital write message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
* @param valueToSet: digital value to set (must be DIGITAL_LOW or DIGITAL_HIGH)
*/
void encodeDigitalWrite(byte output[], int * size,int pinNum, byte valueToSet);

/**
* decode analog write message
* @param input: message to decode
* @param pinNum: pin number decoded
* @param valueToSet: analog value to set (must be [0..255] for arduino)
* @return -1 if a pb occured, 0 otherwise
*/
int decodeAnalogWrite(byte input[], int * pinNum, byte * valueToSet);

/**
* encode analog write message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
* @param valueToSet: digital value to set (must be [0..255] for arduino)
*/
void encodeAnalogWrite(byte output[], int * size,int pinNum, int valueToSet);

/**
* decode digital read message
* @param input: message to decode
* @param pinNum: pin number decoded
* @return -1 if a pb occured, 0 otherwise
*/
int decodeDigitalRead(byte input[], int * pinNum);

/**
* encode digital read message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
*/
void encodeDigitalRead(byte output[],int * size, int  pinNum);

/**
* decode analog read message
* @param input: message to decode
* @param pinNum: pin number decoded
* @return -1 if a pb occured, 0 otherwise
*/
int decodeAnalogRead(byte input[], int * pinNum);

/**
* encode analog read message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
*/
void encodeAnalogRead(byte  output[], int * size, int  pinNum);

/**
* decode digital read value message
* @param input: message to decode
* @param pinNum: pin number decoded
* @param valueRead: value read decoded
* @return -1 if a pb occured, 0 otherwise
*/
int decodeDigitalReadValue(byte input[], int * pinNum, byte * valueRead);

/**
* encode digital read value message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
* @param valueRead: value read encoded
*/
void encodeDigitalReadValue(byte  output[], int * size, int pinNum, int valueRead);
    
 /**
* decode analog read value message
* @param input: message to decode
* @param pinNum: pin number decoded
* @param valueRead: value read decoded
* @return -1 if a pb occured, 0 otherwise
*/   
int decodeAnalogReadValue(byte input[], int * pinNum, int * valueRead);

/**
* encode analog read value message
* @param output: encoded message
* @param size: size of the encoded message
* @param pinNum: pin number 
* @param valueRead: value read encoded
*/
void encodeAnalogReadValue(byte  output[], int * size, int pinNum, int valueRead);
    



class IrisClass
{
public:
	IrisClass();
	
	boolean available(); // new data available
	void decodeMessage();
	
    // request: MASTER -> SLAVE
    virtual void debugMsgReq(int id);
    
    virtual void pinModeReq(int pinMode, byte mode);        
    virtual void digitalWriteReq(int pinMode, byte valueToSet);  
    virtual void analogWriteReq(int pinMode, int valueToSet);
    virtual void digitalReadReq(int pinMode);
    virtual void analogReadReq(int pinMode);

            
protected:
	int                _i2cAdress; // I2C Address of the device
	MsgContainer *     _ptMsgContainerQueue;
	IrisBehaviour *    _irisBehaviour;
	
    void treatPinModeMsg(byte data[], int msgSize);
    
	void treatDigitalWriteMsg(byte data[], int msgSize);
	void treatDigitalWritesMsg(byte data[], int msgSize);
		
	void treatAnalogWriteMsg(byte data[], int msgSize);
	void treatIdlVersionMsg(byte data[], int msgSize);
	
	void treatDigitalReadMsg(byte data[], int msgSize);
	void treatAnalogReadMsg(byte data[], int msgSize);
    
    
	virtual void printDebug(const String &functionName, const String &strToPrint);
};

extern MsgContainer     g_MsgContainerQueue; // this will contain msg received from Master
extern byte             g_pOutputMsgBuf[ANSWER_MAX_SIZE]; // will contain prepared answer to Master
extern int              g_OutputMsgSize;    // effective size of the answer

#endif