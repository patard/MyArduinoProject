#ifndef IrisBehaviour_h
#define IrisBehaviour_h



/*
 Interface which defines behaviour.
 Shall be implemented by inheritage 
*/

class IrisBehaviour
{
public:
	IrisBehaviour();
	
	void setPinMode(int pinNumber, int mode);
	
};

extern IrisBehaviour IrisBehav;

#endif