/* 
* PWM.cpp
*
* Created: 01/08/2019 23:17:47
* Author: Renan
*/


#include "PWM.h"



// default constructor
PWM::PWM(Digital *port)
{
	this->port = port;
} //PWM

// default destructor
PWM::~PWM()
{
} //~PWM


void PWM::enable(){
	if( Tcounter == 0) Tcounter = 1;
}

void PWM::disable(){
	Tcounter = 0;
	port->clear();
}

bool PWM::isEnabled(){
	if( Tcounter )return true;
	return false;
}

void PWM::interrupt(){
	if( Tcounter > 1 ){
		Tcounter--;
		if( Tcounter == pwm )
			port->set();
	}else if( Tcounter == 1 ){
		port->clear();
		Tcounter = 11;
	}
}

void PWM::setPWM(uint8_t pwm){
	if( pwm == 0)
		disable();
	else{
		//enable();
	}
	this->pwm = pwm;
}

uint8_t PWM::getPWM(){
	return pwm;
}

void PWM::incPWM(){
	if(pwm < 10) pwm++;
	//if( pwm > 11 ) pwm = 2;
}

void PWM::decPWM(){
	if( pwm > 2) pwm--;
	//if( pwm > 11 ) pwm = 2;
}