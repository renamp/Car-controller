/* 
* AnalogExp.cpp
*
* Created: 06/06/2020 14:43:57
* Author: Renan
*/


#include "AnalogExp.h"

// default constructor
//AnalogExp::AnalogExp(uint8_t *_rawValue, uint8_t midRange1)
AnalogExp::AnalogExp(uint8_t *_rawValue, uint8_t midRange1, uint8_t midRange2):Analog(_rawValue)
{
	_midRange1 = midRange1;
	_midRange2 = midRange2;
} //AnalogExp

uint8_t AnalogExp::calcExp(uint8_t value){
	int n=0;
	long m=0;
	if( value <= _midP1 ){
		m = _midRange1;
		m = (m*100)/_midP1;
		n = _midRange1;
		n = n-((m*_midP1)/100);
	}
	else if( value <= _midP2 ){
		m = _midRange2 - _midRange1;
		m = (m*100)/(_midP2-_midP1);
		long tmpN = _midRange2;
		tmpN = tmpN - ((m*_midP2)/100);
		n = (int)tmpN;
	}
	else {
		m = 255 - _midRange2;
		m = (m*100)/(255-_midP2);
		long tmpN = 255-((m*255)/100);
		n = (int)tmpN;
	}
	
	long Result = value;
	Result = ((Result*m)/100) + n;
	return (uint8_t)Result;
}

void AnalogExp::Update(){
	if( rawValue() > _maxRange) _maxRange = rawValue();
	
	Analog::Update();
	_value = calcExp(_value);
}