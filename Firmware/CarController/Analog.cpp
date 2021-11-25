/* 
* Analog.cpp
*
* Created: 10/05/2020 17:13:13
* Author: Renan
*/


#include "Analog.h"

// default constructor
Analog::Analog(uint8_t *_rawValue)
{
	Analog::_rawValue = _rawValue;
	_minRange = 0;
	_maxRange = 255;
} //Analog

// default destructor
Analog::~Analog()
{
} //~Analog


uint8_t Analog::rawValue()
{
	return *Analog::_rawValue;
}

void Analog::Update()
{
	uint16_t tmp1=0, tmp2;
	if( (*_rawValue) > _minRange ) tmp1 = (*_rawValue) - _minRange;
	
	tmp1 *= 255;
	tmp2 = _maxRange - _minRange;
	tmp1 = tmp1/tmp2;
	if( tmp1 > 255 ) tmp1 = 255;
	_value = (uint8_t)tmp1;
}

uint8_t Analog::Value()
{
	return _value;
}

void Analog::setRange(uint8_t minRange, uint8_t maxRange)
{
	_minRange = minRange;
	_maxRange = maxRange;
}

void Analog::setRange(uint8_t maxRange)
{
	_minRange = rawValue()+2;
	int tmpMax = maxRange;
	tmpMax += (int(_minRange));
	if( tmpMax > 255 ) _maxRange = 255;
	else _maxRange = (uint8_t)tmpMax;
}

uint8_t Analog::getMinRange()
{
	return _minRange;
}

uint8_t Analog::getMaxRange()
{
	return _maxRange;
}