/* 
* PWM.h
*
* Created: 01/08/2019 23:17:48
* Author: Renan
*/


#ifndef __PWM_H__
#define __PWM_H__

#include <avr/io.h>
#include "Digital.h"

class PWM
{
//variables
public:
	
protected:
private:
	Digital *port;
	uint8_t Tcounter;				// timer counter
	uint8_t pwm;
	
//functions
public:
	PWM(Digital *port);
	void enable();
	void disable();
	bool isEnabled();
	void setPWM(uint8_t pwm);
	uint8_t getPWM();
	void incPWM();
	void decPWM();
	void interrupt();
	~PWM();
protected:
private:

};

#endif //__PWM_H__
