/* 
* Digital.h
*
* Created: 23/11/2019 21:23:42
* Author: Renan
*/


#ifndef __DIGITAL_H__
#define __DIGITAL_H__

#include <avr/io.h>
#include "Bits.h"


enum MODE{modeINPUT=0, modeOUTPUT=1};

class Digital
{
//variables
public:
protected:
private:
	volatile uint8_t *PORT;
	volatile uint8_t *DDR;
	volatile uint8_t *PIN;
	uint8_t pin;
	MODE mode;
	
//functions
public:
	Digital(volatile uint8_t *Port, uint8_t Pin, MODE mode);
	~Digital();
	void set();
	void clear();
	void toogle();
	void write(bool value);
	bool read();
	
protected:
private:

}; //Digital

#endif //__DIGITAL_H__
