/*
Iris.h - Iris library v0.6 - 2016-6-26
Copyright (c) 2016 Thales Services. All rights reserved.
I2C communication in which Arduino is the slave
*/
#ifndef Iris_h
#define Iris_h

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include <Arduino.h>
#include <Wire.h> // I2C library
#else
//TODO define byte
#endif

#include "MsgContainer.h"
#include "IrisBehaviour.h"

/* Version numbers for the protocol. The protocol is still changing, so these
* version numbers are important.
* Query using the REPORT_VERSION message.
*/
#define Iris_PROTOCOL_MAJOR_VERSION 0
#define Iris_PROTOCOL_MINOR_VERSION 6
/* Version numbers for the Iris library.
* The firmware version will not always equal the protocol version going forward.
* Query using the REPORT_FIRMWARE message.
*/
#define Iris_FIRMWARE_MAJOR_VERSION 0
#define Iris_FIRMWARE_MINOR_VERSION 2
// message command bytes
#define PIN_MODE_MSG_ID 0x01
#define DIGITAL_READ_MSG_ID 0x02
#define DIGITAL_WRITE_MSG_ID 0x03
#define DIGITAL_WRITES_MSG_ID 0x04
#define ANALOG_READ_MSG_ID 0x05
#define ANALOG_WRITE_MSG_ID 0x06
#define DEFINE_ENCODER_MSG_ID 0x07
#define GET_ENCODER_COUNTER_MSG_ID 0x08
#define GET_SOFT_VERSION_MSG_ID 0x09
#define GET_STATUS_MSG_ID 0x0A
#define GET_TYPE_ARDUINO_MSG_ID 0x0B
#define GET_IDL_VERSION_MSG_ID 0x0C

#define DIGITAL_READ_VALUE_MSG_ID 0xF2
#define ANALOG_READ_VALUE_MSG_ID 0xF5
#define ENCODER_COUNTER_MSG_ID 0xF8
#define STATUS_MSG_ID 0xFA
#define TYPE_ARDUINO_MSG_ID 0xFB
#define IDL_VERSION_MSG_ID 0xFC
#define SOFT_VERSION_MSG_ID 0xF9

// Digital pin mode (Iris protocol value)
#define INPUT_PIN_MODE 0
#define OUTPUT_PIN_MODE 1
#define PULLUP_PIN_MODE 2

// Digital pin value (Iris protocol value)
#define DIGITAL_LOW 0
#define DIGITAL_HIGH 1
#define UNRELEVANT_I2C_ADDR -1

class IrisClass
{
public:
	IrisClass();
	
	boolean available(); // new data available
	void decodeMessage();
	
	static void decodeDigitalRead(byte aByte, int * pinNum, int * valueRead);
	
protected:
	int _i2cAdress; // I2C Address of the device
	MsgContainer * _ptMsgContainerQueue;
	IrisBehaviour * _irisBehaviour;
	
	void treatDigitalWriteMsg(byte data[], int msgSize);
	void treatDigitalWritesMsg(byte data[], int msgSize);
	void treatPinModeMsg(byte data[], int msgSize);
	
	void treatAnalogWriteMsg(byte data[], int msgSize);
	void treatIdlVersionMsg(byte data[], int msgSize);
	
	void digitalReadMsg(byte data[], int msgSize);
	void analogReadMsg(byte data[], int msgSize);
	
	void printDebug(const String &functionName, const String &strToPrint);
};

extern MsgContainer g_MsgContainerQueue;
extern byte g_pOutputMsgBuf[10]; // buffer ecriture
extern int g_OutputMsgSize;
#endif