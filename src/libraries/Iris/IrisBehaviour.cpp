#include "IrisBehaviour.h"
#include "Iris.h" // for Digital pin mode def

IrisBehaviour::IrisBehaviour()
{
}

void IrisBehaviour::setPinMode(int pinNumber, int pinModeToSet)
{
	byte arduinoPinMode = OUTPUT ; // default initialization
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
	::pinMode(pinNumber, arduinoPinMode);
}

void IrisBehaviour::digitalWrite(int pinNumber, int valueToSet)
{
	//Serial.println(__func__);
    
    byte arduinoValue = HIGH; // default initialization
	if (valueToSet == DIGITAL_LOW) {
		arduinoValue = LOW;
	}
	// call Arduino core
	::digitalWrite(pinNumber, arduinoValue);
}


/*void IrisBehaviour::sendMsg(byte msg[])
{
}*/

void IrisBehaviour::analogWrite(int pinNumber, int value)
{
	// TODO check if value [0..255] as it is PWM
	if ( value < 0 ) {
		value = 0;
	}
	else if ( value > 255 ) {
		value = 255;
	}
	
	::analogWrite(pinNumber, value);
}


bool IrisBehaviour::digital_Read(int pinNumber)
{
	// TODO check if on Arduino it is really a digital Pin
    Serial.print("DR on Pin ");
    Serial.print(pinNumber);
    Serial.print(" ,value ");
    Serial.println(digitalRead(pinNumber));
	return digitalRead(pinNumber);
}

int IrisBehaviour::analogRead(int pinNumber)
{
	// TODO check if on Arduino it is really an analog Pin
	return ::analogRead(pinNumber);
}

IrisBehaviour IrisBehav;