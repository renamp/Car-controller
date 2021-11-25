/* 
* Timer.cpp
*
* Created: 10/05/2020 22:39:13
* Author: Renan
*/


#include "Timer.h"

// default constructor
Timer::Timer(void (*func)(), TMODE mode)
{
	Timer::_funcUpd = (*func);
	Timer::Status = mode;
}
Timer::Timer(void (*func)())
{
	Timer::_funcUpd = (*func);
}
Timer::Timer(){
}

// default destructor
Timer::~Timer()
{
} //~Timer


void Timer::setTime(uint8_t time){
	Ttimer = time;
}

void Timer::setFunc(void (*func)()){
	Timer::_funcUpd = (*func);
}

void Timer::timer()
{
	if(Tcounter > 1 )Tcounter--;
	if(testbit(Status, 0) && Tcounter == 0) Tcounter = Ttimer;
}

void Timer::update()
{
	if( Tcounter == 1 ){
		if( testbit(Status, 0) && testbit(Status, 1) ) Tcounter = Ttimer;
		Tcounter--;
		_funcUpd();
	}
} //Timer

void Timer::enable(){
	setbit(Status, 0);
	if( !testbit(Status, 1) ) Tcounter = Ttimer;
}

void Timer::disable(){
	clearbit(Status, 0);
	Tcounter = 0;
}