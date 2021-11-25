/* 
* AnalogExp.h
*
* Created: 06/06/2020 14:43:57
* Author: Renan
*/


#ifndef __ANALOGEXP_H__
#define __ANALOGEXP_H__

#include "avr/io.h"
#include "Analog.h"

class AnalogExp : public Analog
{
//variables	
public:
protected:
private:
	uint8_t _midRange1, _midRange2;
	static const uint16_t _midP1 = 90;
	static const uint16_t _midP2 = 170;

//functions
public:
	//AnalogExp(uint8_t *_rawValue);
	AnalogExp(uint8_t *_rawValue, uint8_t midRange1, uint8_t midRange2);
	void Update();	// override
	uint8_t calcExp(uint8_t value);
protected:
private:
	

}; //AnalogExp

#endif //__ANALOGEXP_H__
