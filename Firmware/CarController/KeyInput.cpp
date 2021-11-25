/* 
* KeyInput.cpp
*
* Created: 13/06/2020 18:24:45
* Author: Renan
*/


#include "KeyInput.h"

// default constructor
KeyInput::KeyInput()
{
} //KeyInput

// default destructor
//KeyInput::~KeyInput()
//{
//} //~KeyInput


void KeyInput::pressUp(){
	if( pressing ){
		if( duration > 600 )pressed = true;
		pressing = false;
	}
}
void KeyInput::pressDown(){
	if( !pressed ){
		pressing = true;
		duration++;
	}
}
void KeyInput::release(){
	if( pressed ){
		pressed = false;
		duration = 0;
	}
}