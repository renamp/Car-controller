/* 
* EngineControlUnit.h
*
* Created: 16/05/2020 22:01:54
* Author: Renan
*/


#ifndef __ENGINECONTROLUNIT_H__
#define __ENGINECONTROLUNIT_H__

#include <avr/io.h>
#include "Bits.h"
#include "Digital.h"

class EngineControlUnit
{
//variables
public:
	Digital *MotorR, *MotorL;
	uint8_t TargetSPD;				// target Speed
	uint8_t Tacceleration;			// acceleration rate
	uint8_t maxSPD;					// maximum speed
	bool Enabled;
protected:
private:
	uint8_t ActualSPD;				// actual Speed
	uint8_t Tcounter;				// timer counter
	volatile uint8_t *TOCR;			// Interruption compare value
	uint8_t status;					// control for interruptions
	enum STS_MODE{
		STS_IDLE	=0x00,			// do nothing (null)
		STS_MR		=0x00,			// Motor R (bit)
		STS_ML		=0x01,			// Motor L (bit)
		STS_MRL		=0x02			// Motor R and L (bit)
	};
//functions
public:
	EngineControlUnit(Digital *MotorR, Digital *MotorL, volatile uint8_t *TOCR);
	~EngineControlUnit();
	void InterruptOVF();
	void InterruptCOMP();
	void setTargetSPD(uint8_t _TargetSPD);
protected:
private:
}; //EngineControlUnit

#endif //__ENGINECONTROLUNIT_H__
