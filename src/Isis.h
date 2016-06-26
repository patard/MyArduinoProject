/*
  Isis.h - Isis library v0.6 - 2016-6-26
  Copyright (c) 2016 Thales Services.  All rights reserved.

*/

#ifndef Isis_h
#define Isis_h

#include "Boards.h"  /* Hardware Abstraction Layer + Wiring/Arduino */

/* Version numbers for the protocol.  The protocol is still changing, so these
 * version numbers are important.
 * Query using the REPORT_VERSION message.
 */
#define ISIS_PROTOCOL_MAJOR_VERSION 0
#define ISIS_PROTOCOL_MINOR_VERSION 6

/* Version numbers for the Isis library.
 * The firmware version will not always equal the protocol version going forward.
 * Query using the REPORT_FIRMWARE message.
 */
#define ISIS_FIRMWARE_MAJOR_VERSION 0
#define ISIS_FIRMWARE_MINOR_VERSION 2





#define BUFFER_SIZE_MAX 4 // max number of data bytes

// message command bytes
#define PIN_MODE_MSG_ID  			0x01
#define DIGITAL_READ_MSG_ID  		0x02
#define DIGITAL_WRITE_MSG_ID  		0x03
#define DIGITAL_WRITES_MSG_ID  		0x04
#define ANALOG_READ_MSG_ID  		0x05
#define ANALOG_WRITE_MSG_ID  		0x06
#define DEFINE_ENCODER_MSG_ID  		0x07
#define GET_ENCODER_COUNTER_MSG_ID 	0x08
#define GET_SOFT_VERSION_MSG_ID 	0x09
#define GET_STATUS_MSG_ID 			0x0A
#define GET_TYPE_ARDUINO_MSG_ID 	0x0B
#define GET_IDL_VERSION_MSG_ID 		0x0C

#define DIGITAL_READ_VALUE_MSG_ID  	0xF2
#define ANALOG_READ_VALUE_MSG_ID  	0xF5
#define ENCODER_COUNTER_MSG_ID 		0xF8
#define STATUS_MSG_ID 				0xFA
#define TYPE_ARDUINO_MSG_ID 		0xFB
#define IDL_VERSION_MSG_ID 			0xFC
#define SOFT_VERSION_MSG_ID 		0xF9


typedef struct EncoderStruct
{
  int Id;
  int pinA;
  int pinB;
  int tick_counter;
  void (*ptr_InterruptA) (void); // callback associé au capteur A
  void (*ptr_InterruptB) (void); // callback associé au capteur B
  byte cptA, prevCptA; // Compteurs d'interruption A
  byte cptB, prevCptB; // Compteurs d'interruption B
} EncoderStruct;

EncoderStruct g_pEncoderTab[NB_ENCODER_MAX];



class IsisClass
{
  public:
    IsisClass();
    /* Arduino constructors */
    void begin();
    void begin(long);
    void begin(Stream &s);

    void printVersion();
    void blinkVersion();
	
	void setFirmwareNameAndVersion(const char *name, byte major, byte minor);

	void getIdlVersionMsg_received();
	void pinModeMsg_received(byte* p_pData, int p_MsgSize);
	void digitalWriteMsg_received(byte* p_pData, int p_MsgSize);
	void digitalWritesMsg_received(byte* p_pData, int p_MsgSize);
	void analogWriteMsg_received(byte* p_pData, int p_MsgSize);
	void defineEncoderMsg_received(byte* p_pData, int p_MsgSize);
	void getEncoderCounterMsg_received(byte* p_pData, int p_MsgSize);

	void digitaleReadMsg_received(byte* p_pData, int p_MsgSize);
	void analogReadMsg_received(byte* p_pData, int p_MsgSize);

	void getSoftVersionMsg_received();
	void getStatusMsg_received();
	void getTypeArduinoMsg_received();

	void display_encoder_info(int p_Id);
	void display_encoder_info_light(int p_Id);

	void display_all_Encoder_info_light();

private:

	Stream *IsisStream;
    /* firmware name and version */
    byte firmwareVersionCount;
    byte *firmwareVersionVector;

	void prepareMsg2Send(byte* p_pData, int p_MsgSize);

	byte _Status = 3;

	byte _pInputMsgBuf[BUFFER_SIZE_MAX];
	byte _pOutputMsgBuf[BUFFER_SIZE_MAX];
	int _OutputMsgSize = 0;

	byte _EncoderCounterIdRequest = 0;
	byte _DigitalePinReadRequest = 0;
	byte _AnalogPinReadRequest = 0;
};

extern IsisClass Isis;

/*==============================================================================
 * MACROS
 *============================================================================*/

/* shortcut for setIsisNameAndVersion() that uses __FILE__ to set the
 * firmware name.  It needs to be a macro so that __FILE__ is included in the
 * firmware source file rather than the library source file.
 */
#define setIsisVersion(x, y)   setIsisNameAndVersion(__FILE__, x, y)

#endif