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
	pinMode(pinNumber, arduinoPinMode);
}

IrisBehaviour IrisBehav;