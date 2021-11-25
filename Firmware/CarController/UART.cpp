/*
 * UART.c
 *
 * Created: 25/07/2013 22:17:30
 *  Author: Renan
 */ 

#include "UART.h"

void UART_init( unsigned long BAUD ){
	unsigned long ubrr;
	
	UCSRA = 0x02;
	UCSRB = (1<<TXEN) | (1<<RXEN);
	UCSRC = (3<<UCSZ0);
	
	ubrr = ((F_CPU/8)/BAUD)-1;
	UBRRH = (unsigned char) (ubrr>>8);
	UBRRL = (unsigned char) (ubrr);
}

void UART_Write( unsigned char Data ){
	while( !(UCSRA & (1<<UDRE)) );
	UDR = Data;
}

void UART_Print( const char str[] ){
	while( *str != 0 ){
		UART_Write(*str);
		str++;
	}
}

unsigned char UART_Read( ){
	if( UCSRA & (1<<RXC)){
		return UDR;
	}
	return 0;
}

char UART_DataIsReady( ){
	if( UCSRA & (1<<RXC) ) return 1;
	return 0;
}

void UART_waitData(){
	while( !( UCSRA & (1<<RXC) ) );
}