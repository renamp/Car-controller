/* 
* KeyInput.h
*
* Created: 13/06/2020 18:24:46
* Author: Renan
*/


#ifndef __KEYINPUT_H__
#define __KEYINPUT_H__

#include <avr/io.h>

class KeyInput
{
//variables
public:
	bool pressed, pressing;
	uint16_t duration;
	
protected:
private:

//functions
public:
	KeyInput();
//	~KeyInput();
	void pressUp();
	void pressDown();
	void release();
protected:
private:
//	KeyInput( const KeyInput &c );
//	KeyInput& operator=( const KeyInput &c );

}; //KeyInput

#endif //__KEYINPUT_H__
