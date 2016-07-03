#ifndef IrisBehaviour_h
#define IrisBehaviour_h

#include <Arduino.h>


/*
Interface which defines behaviour.
Shall be implemented by inheritage
*/
class IrisBehaviour
{
public:
	IrisBehaviour();
    
	void setPinMode(int pinNumber, int mode);
	
	void digitalWrite(int pinNumber, int value);
	void analogWrite(int pinNumber, int value);
	
	bool digitalRead(int pinNumber);
	int analogRead(int pinNumber);
};

extern IrisBehaviour IrisBehav;
#endif