/*
 * UART.h
 *
 * Created: 25/07/2013 22:15:55
 *  Author: Renan
 */ 

#ifndef F_CPU
	#define F_CPU 8000000
#endif

#include "avr/io.h"

void UART_init( unsigned long BAUD);

void UART_Write( unsigned char Data );

char UART_DataIsReady( );

void UART_waitData();

unsigned char UART_Read( );

//void UART_Print( char str[] );
void UART_Print( const char str[] );