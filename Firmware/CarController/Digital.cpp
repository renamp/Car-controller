/* 
* Digital.cpp
*
* Created: 23/11/2019 21:23:42
* Author: Renan
*/


#include "Digital.h"

// default constructor
Digital::Digital(volatile uint8_t *Port, uint8_t Pin, MODE mode)
{
	this->pin = Pin;
	this->mode = mode;
	this->PORT = Port;
	
	this->DDR = Port;			// save PORT
	this->DDR--;				// save DDR
	this->PIN = DDR;
	this->PIN--;				// save PIN
	
	if( this->mode )
		setbit(*this->DDR, this->pin);
	else 
		clearbit(*this->DDR, this->pin);
} //Digital

void Digital::set(){
	setbit(*PORT, pin);
}
void Digital::clear(){
	clearbit(*PORT, pin);
}
void Digital::toogle(){
	//setbit(*PIN, pin);
	if( testbit(*PORT, pin) )
		clearbit(*PORT, pin);
	else
		setbit(*PORT, pin);
}
void Digital::write(bool value){
	if( value )
		setbit(*PORT, pin);
	else 
		clearbit(*PORT, pin);
}
bool Digital::read(){
	return testbit(*PIN, pin);
}

// default destructor
Digital::~Digital()
{
} //~Digital
