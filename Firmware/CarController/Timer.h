/* 
* Timer.h
*
* Created: 10/05/2020 22:39:13
* Author: Renan
*/


#ifndef __TIMER_H__
#define __TIMER_H__

#include "avr/io.h"
#include "Bits.h"

enum TMODE{TM_FreeRun=0x02, TM_Timer=0x00, TM_Enabled=0x01};

class Timer
{
//variables
public:
	uint8_t Ttimer;
	void (*_funcUpd)(void);
	
protected:
private:
	uint8_t Status;  // b0:Enabled/Disabled  b1:FreeRun/Timer
	uint8_t Tcounter;
	
//functions
public:
	Timer(void (*_funcUpd)(void), TMODE mode);
	Timer(void (*_funcUpd)(void));
	Timer();
	~Timer();
	
	void setTime(uint8_t time);
	void setFunc(void (*_funcUpd)(void));
	void timer();
	void update();
	void enable();
	void disable();
	
protected:
private:

}; //Timer

#endif //__TIMER_H__
