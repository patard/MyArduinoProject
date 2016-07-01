#ifndef IrisArduino_h
#define IrisArduino_h

#include "Iris.h"
#include <Arduino.h>

/* That is a class function (method) which has an implicit this pointer. You can't use it as a static ISR.
In general, classes cannot implement ISRs for this reason. There are a few workarounds, one being to make it a static class function.
However then it will affect the entire class, not just one instance. */
void sendData(); // callback
void receiveData(int numBytes);

class IrisArduinoClass: public IrisClass
{
public:
	IrisArduinoClass();
	/* Application of APIStyleGuide of Arduino:
	Use begin() to initialize a library instance, usually with some settings */
	void begin();
	void begin(int i2cAddress);
	void printDebug(const String &functionName, const String &strToPrint);
protected:
	void initI2cAsSlave(int i2cAddress);
};

extern IrisArduinoClass Iris;
#endif