/*
* Implementation of Iris on Arduino
*/

#ifndef IrisArduino_h
#define IrisArduino_h

#include "Iris.h"
#include <Arduino.h>

/* That is a class function (method) which has an implicit this pointer. You can't use it as a static ISR.
In general, classes cannot implement ISRs for this reason. There are a few workarounds, one being to make it a static class function.
However then it will affect the entire class, not just one instance. */

/**
* 
*/
void sendData(); // callback

/**
* function call on interrupt genrated by I2C message reception
* @param numBytes: number of byes receives (cf arduino Wire library for more explanation)
*/
void receiveData(int numBytes);

class IrisArduinoClass: public IrisClass
{
public:
	IrisArduinoClass();
	/* Application of APIStyleGuide of Arduino:
	Use begin() to initialize a library instance, usually with some settings */
	void begin();
	void begin(int i2cAddress);
    void beginMaster(int i2cAddress);

    void debugMsgReq(int id);
    void pinModeReq(int pinNum, byte mode);
    
    void digitalWriteReq(int pinNum, byte valueToSet);
    void digitalReadReq(int pinNum);
    
    void analogWriteReq(int pinNum, int valueToSet);
    void analogReadReq(int pinMode);
    
    //void setFirmwareNameAndVersion(const char *name, byte major, byte minor);
    void blinkVersion(void);
    
protected:
	void initI2cAsSlave(int i2cAddress);
    
    void printDebug(const String &functionName, const String &strToPrint);
    
    
    /* firmware name and version */
    //byte _firmwareVersionCount;
    //byte *_firmwareVersionVector;
    void strobeBlinkPin(byte pin, int count, int onInterval, int offInterval);
    
};

extern IrisArduinoClass Iris;


/*==============================================================================
 * MACROS
 *============================================================================*/

/* shortcut for setFirmwareNameAndVersion() that uses __FILE__ to set the
 * firmware name.  It needs to be a macro so that __FILE__ is included in the
 * firmware source file rather than the library source file.
 */
#define setFirmwareVersion(x, y)   setFirmwareNameAndVersion(__FILE__, x, y)

#endif