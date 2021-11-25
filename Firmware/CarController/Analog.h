/* 
* Analog.h
*
* Created: 10/05/2020 17:13:13
* Author: Renan
*/


#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "avr/io.h"

class Analog
{
//variables
public:
	uint8_t *_rawValue;
protected:
	uint8_t _value, _maxRange;
private:
	uint8_t _minRange;
	

//functions
public:
	Analog(uint8_t *_rawValue);
	~Analog();
	void Update();
	uint8_t rawValue();
	uint8_t Value();
	void setRange(uint8_t minRange, uint8_t maxRange);
	void setRange(uint8_t maxRange);
	
	uint8_t getMinRange();
	uint8_t getMaxRange();
	
protected:
private:

}; //Analog

#endif //__ANALOG_H__
