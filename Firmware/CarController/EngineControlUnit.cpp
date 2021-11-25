/* 
* EngineControlUnit.cpp
*
* Created: 16/05/2020 22:01:54
* Author: Renan
*/


#include "EngineControlUnit.h"


// default constructor
EngineControlUnit::EngineControlUnit(Digital *MotorR, Digital *MotorL, volatile uint8_t *TOCR)
{
	this->MotorR = MotorR;
	this->MotorL = MotorL;
	this->TOCR   = TOCR;
	status = STS_IDLE;
	maxSPD = 255;
	Enabled = true;
} //EngineControlUnit

// default destructor
EngineControlUnit::~EngineControlUnit()
{
} //~EngineControlUnit


void EngineControlUnit::InterruptOVF(){
	if(Tcounter > 1 )Tcounter--;
	else{
		Tcounter = Tacceleration;
		
		if( TargetSPD > ActualSPD )			// Acceleration
			ActualSPD++;
		else if( TargetSPD < ActualSPD ){	// Deceleration
			if( TargetSPD > 100)			//   slow deceleration 
				ActualSPD--;
			else if( TargetSPD > 50)		//   fast decelerate
				Tcounter = 0, ActualSPD--;
			else if( TargetSPD < 5 )		//   stop
				ActualSPD = TargetSPD;
		}
	}
	
	if( ActualSPD > 127 ){					// more than 50% pwm
		MotorR->set();					
		MotorL->set();
		status = 1<<STS_MRL;				// use both motors
		(*TOCR) = ActualSPD;
	}
	else if( ActualSPD > 20){				// minimun of 2% pwm
		MotorR->set();					
		MotorL->clear();
		status = 1<<STS_MR;					// use first motorR
		(*TOCR) = ActualSPD;
	}
	else{									// less than 2% pwm
		(*TOCR) = 250;
	}
}

void EngineControlUnit::InterruptCOMP(){
	if( testbit(status, STS_MR) ){			// if using motor R
		(*TOCR) = ActualSPD*2;
		MotorR->clear();
		MotorL->set();
		status = 1<<STS_ML;		
	}
	else if( testbit(status, STS_ML) ){		// if using motor L
		MotorL->clear();
		status = STS_IDLE;
	}
	else if( testbit(status, STS_MRL) ){	// if using motor (R and L)
		if( ActualSPD < 255 ){
			MotorR->clear();
			MotorL->clear();
		}
		status = STS_IDLE;
	}
}

void EngineControlUnit::setTargetSPD(uint8_t _TargetSPD){
	if( _TargetSPD > maxSPD ) TargetSPD = maxSPD;
	else TargetSPD = _TargetSPD;
	if(!Enabled) TargetSPD = 0;
}