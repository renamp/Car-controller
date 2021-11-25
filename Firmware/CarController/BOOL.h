/* 
* BOOL.h
*
* Created: 31/05/2020 10:33:40
* Author: Renan
*/


#ifndef __BOOL_H__
#define __BOOL_H__

#include "avr/io.h"
#include "Bits.h"

class BOOL
{
//variables
public:
	const uint8_t *var;
protected:
private:
	
	//uint8_t * const var;
//	const uint8_t bit;

//functions
public:
	//BOOL(uint8_t *_var, const uint8_t _bit);
	//BOOL(uint8_t *_var, uint8_t _bit);
	BOOL(uint8_t *_var);
	~BOOL();
	
	void set();
	void clear();
	uint8_t Value();
	
protected:
private:
}; //BOOL

#endif //__BOOL_H__
