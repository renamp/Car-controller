/*
 * CarController.cpp
 *
 * Created: 01/08/2019 22:51:32
 *  Author: Renan
 */ 

#define F_CPU 8000000

#define TIME_CICLY 2000								// microseconds
#define TIME_INTERRUPT (TIME_CICLY/(1024/8))
#define VOLT_CONVERSION 15

#define PANEL_KEY_RPT_NUM	300
#define VOLTAGE_RPT_NUM		1000
#define KEY_IGNITION_NUM	1000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "Bits.h"
#include "UART.h"
#include "PWM.h"
#include "Digital.h"
#include "Analog.h"
#include "Timer.h"
#include "BOOL.h"
#include "AnalogExp.h"
#include "KeyInput.h"
#include "EngineControlUnit.h"


//*************************************************
// Functions prototypes
void funcReport();
void Functions(uint8_t func);
void TF1();
void TF2();
void TF3();
//*************************************************

//*************************************************
// Ports definition
Digital DlightFront	= Digital(&PORTB, 6, modeOUTPUT);
Digital DlightLeft	= Digital(&PORTB, 7, modeOUTPUT);
Digital DlightRigth	= Digital(&PORTD, 5, modeOUTPUT);
Digital Dbuzzer		= Digital(&PORTB, 6, modeOUTPUT);

Digital MotorR = Digital(&PORTD, 2, modeOUTPUT);				// Motor Rigth 
Digital MotorL = Digital(&PORTD, 3, modeOUTPUT);				// Motor Left
//*************************************************

//*************************************************
// Analog inputs
uint8_t ArawValue[8];
AnalogExp Athrottle		= AnalogExp(&ArawValue[0], 40,100);
Analog Aleft			= Analog(&ArawValue[1]);
Analog Arigth			= Analog(&ArawValue[2]);
Analog Amiddle			= Analog(&ArawValue[3]);
Analog AbuttonsPanel	= Analog(&ArawValue[7]);
Analog AtripotPanel		= Analog(&ArawValue[6]);
//*************************************************

//************************************************
// Timers
Timer T1 = Timer(TF1, TM_FreeRun);
Timer T2 = Timer(TF2, TM_FreeRun);
Timer T3 = Timer(TF3);
// variables
uint8_t TVblink = 0;				// Timer variable to control blink
const uint8_t TCblink = 14;			// Timer number of times to blink
//*************************************************

//************************************************
// Engine controler
EngineControlUnit ecu = EngineControlUnit(&MotorR, &MotorL, &OCR2);
//*************************************************

//************************************************
// lamp controller
PWM lamp = PWM(&DlightFront);
//*************************************************

//************************************************
// Key Input
KeyInput keyLeft, keyRigh, keyMiddle, keyButton;
bool keySwitch, keyIgnitionP1, keyIgnitionP2;
bool keyIgnitionP1_old, keyIgnitionP2_old;
//*************************************************

//************************************************
// function variables
uint16_t PanelKeyRPT, VoltageRPT, KeyIgnition1RPT, KeyIgnition2RPT, keyIgnitionMode;
uint8_t stsLamps;
#define BOOL_VAR1 stsLamps
#define BOOL_LAMP_LEFT_V1	0
#define BOOL_LAMP_RIGTH_V1	1
#define BOOL_LAMP_ALERT_V1	2
#define BOOL_LAMP_FRONT_V1	3
#define BOOL_BAT_LOW1_V1	4
//*************************************************

//************************************************
// debug variables
bool report1;
//*************************************************


char txt[10];
char* IntToStr( int readValue ){
	int txtindex = 0;
	
	if( readValue/10000)
		txt[txtindex++] = (readValue/10000)+0x30; readValue = readValue%10000;
	if( readValue/1000)
		txt[txtindex++] = (readValue/1000)+0x30; readValue = readValue%1000;
	//if( readValue/100)
	txt[txtindex++] = (readValue/100)+0x30; readValue = readValue%100;
	txt[txtindex++] = (readValue/10)+0x30;
	txt[txtindex++] = (readValue%10)+0x30;
	//txt[txtindex++] = '\n';
	txt[txtindex] = 0;
	
	return txt;
}


//##__BEGIN__############################################################
///////////////////////////////////////
// Initialization of ADC Interrupt
void InitADCInterrupt(){
	ADMUX  = 0x60;
	ADCSRA = 0x8E;
	setbit(ADCSRA, 6);
}
/////////////////////////////////////////
// ADC Interrupt routine
ISR( ADC_vect ){
	ArawValue[ADMUX&0x0F] = ADCH;
}
////////////////////////////////////////
// ADC update
void ADC_update(){
	if( !testbit(ADCSRA, 6) ){
		if(ADMUX == 0x67) ADMUX = 0x60;
		else ADMUX++;
		setbit(ADCSRA, 6);
	}
}
//##__END__###############################################################


//##__BEGIN__############################################################
uint8_t timer=20, _timer;
///////////////////////////////////////
// Initialization of Time Interrupt
void InitTimeInterrupt0(){
	TCCR0	= 0x05;									// Setup Timer with prescaller 1024
	setbit(TIMSK,0);								// Overflow interrupt enable
}
/////////////////////////////////////////
// Interrupt routine timer ( each 32.7ms )
ISR( TIMER0_OVF_vect){
	if(_timer==1){
		_timer = timer;
	}
	else if( _timer > 1 )
	_timer--;

	T1.timer();	
	T2.timer();
	//T3.timer();
}
//##__END__###############################################################


//##__BEGIN__ ############################################################
///////////////////////////////////////
// Initialization of Interrupt
void InitTimeInterrupt2(){
	TCCR2	= 0x04;									// phase correct pwm | 64_prescaller
	setbit(TIMSK,6);								// TOIE2 Overflow interrupt enable
	setbit(TIMSK,7);								// OCIE2 Coparator interrupt enable
}
/////////////////////////////////////////
// Interrupt routine PWM_overflow ( each 2ms )
ISR(TIMER2_OVF_vect){
	ecu.InterruptOVF();
	lamp.interrupt();
}
/////////////////////////////////////////
// Interrupt routine PWM ( max 2ms )
ISR( TIMER2_COMP_vect){
	ecu.InterruptCOMP();
}
//##__END__###############################################################

///////////////////////////////////////////////////////////////
//**************************************************************
// Timer Functions
// Timer function T1 (500ms)
void TF1(){
	//--------------------
	// blink function
	if( TVblink > 1){
		if( TVblink < 255) TVblink--;
		if( testbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1) ){
			DlightRigth.clear();
			DlightLeft.toogle();
		}
		else if( testbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1) ){
			DlightRigth.toogle();
			DlightLeft.clear();
		}
		else if( testbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1) ){
			DlightRigth.toogle();
			DlightLeft.write(DlightRigth.read());
		}
		else if( testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ){
			DlightRigth.toogle();
			if(DlightRigth.read()) DlightLeft.clear();
			else DlightLeft.set();
		}
	}
	else if(TVblink == 1){
		TVblink = 0;
		DlightRigth.clear();
		DlightLeft.clear();
		clearbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
		clearbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
		clearbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1);
	}
	//--------------------
	
	//----------------------
	// check key Ignition
	if(keyIgnitionMode < 1) keyIgnitionMode++;
	else if( keyIgnitionMode == 1 ){
		if( keyIgnitionP1 ){
			//uint8_t value = 255 - AtripotPanel.rawValue();
			//if(value < 40) ecu.maxSPD = 40;
			//else 
				ecu.maxSPD = 120;//AtripotPanel.rawValue();
		}
		if( keyIgnitionP2 ){
			ecu.maxSPD = 255;
		}
		keyIgnitionMode++;
	}
	//------------------
}
// Timer function T2 (2s)
void TF2(){
	if( report1 ) funcReport();
	
	// check voltage level action
	if( VoltageRPT == VOLTAGE_RPT_NUM ){
		ecu.Enabled = false;
		Functions(0x20);
	}
	//else if( testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) )Functions(0x21);
	
}
// Timer function T3 
void TF3(){
	
}
//////////////////////////////////////////////////////////////


void funcReport(){
	UART_Print(IntToStr(ArawValue[0])); UART_Print("\t");
	//UART_Print(IntToStr(ArawValue[1])); UART_Print("\t");
	UART_Print(IntToStr(Aleft.Value())); UART_Print("\t");
	UART_Print(IntToStr(ArawValue[2])); UART_Print("\t");
	UART_Print(IntToStr(ArawValue[6])); UART_Print("\t");
	UART_Print(IntToStr(ArawValue[7])); UART_Print("\t");
	UART_Print("\n");
}


 void Initialization(){

	Athrottle.setRange(80);
	ecu.Tacceleration = 3;
	
	T1.setTime(16);			// set timer (16x32ms)
	T1.enable();			// enable timer
	
	T2.setTime(60);			// set timer (60*x32ms)
	T2.enable();			// enable timer
	
	lamp.setPWM(2);
	
	keyIgnitionMode = 0;
}

void Functions(uint8_t func){
	switch( func ){
		case 0x10:	// blink Left
			if( !(testbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1) || testbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1) )){
				setbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
				TVblink = TCblink;
			}
			break;
		case  0x11:	// blink Right
			if( !(testbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1) || testbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1) )){
				setbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
				TVblink = TCblink;
			}
			break;
		case 0x12:	// blink Alert
			if( !( testbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1) || testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ) ){
				setbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
				TVblink = 255;
			}
			else if( !testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ) {
				TVblink = 1;	// off
			}
			break;
		case 0x13:	// Light
			if( lamp.isEnabled() )
				lamp.setPWM(5);
			else
				lamp.disable();
			break;
			
		case 0x20:
			if( !testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ){
				setbit(BOOL_VAR1, BOOL_BAT_LOW1_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1);
				TVblink = 255;
			}
			break;
		case 0x21:
			if( testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ){
				TVblink = 1;
				DlightRigth.clear();
				DlightLeft.clear();
				clearbit(BOOL_VAR1, BOOL_LAMP_LEFT_V1);
				clearbit(BOOL_VAR1, BOOL_LAMP_RIGTH_V1);
				clearbit(BOOL_VAR1, BOOL_BAT_LOW1_V1);
			}
			break;
	}
}

uint8_t test1(uint8_t raw, uint8_t maxRange){
	uint8_t _minRange = raw+2;
	uint8_t _maxRange;
	int tmpMax = maxRange;
	tmpMax += (int(_minRange));
	if( tmpMax > 255 ) _maxRange = 255;
	else _maxRange = (uint8_t)tmpMax;
	return _maxRange;
}

int main(void)
{	
	UART_init(1000000);
	//UART_init(9600);
	
	cli();
	InitADCInterrupt();
	InitTimeInterrupt0();
	InitTimeInterrupt2();
	sei();
	
	UART_Print("\nRN Car Controller FW v1.0\n");
	_delay_ms(60);
	Initialization();
	
    while(1)
    {
		//***********************
		// Timers
		T1.update();
		T2.update();
		//T3.update();
		
		
		//////////////////////////////////////////////////////////////////////////
		ADC_update();
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		Athrottle.Update();
		ecu.setTargetSPD( Athrottle.Value() );
		//////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////////////
		// Check inputs
		if( Aleft.rawValue() > 200 ) keyLeft.pressDown();		// Left input
		else keyLeft.pressUp();
		if( Arigth.rawValue() > 200 ) keyRigh.pressDown();		// Right input
		else keyRigh.pressUp();
		if( Amiddle.rawValue() > 200 ) keyMiddle.pressDown();	// Middle input
		else keyMiddle.pressUp();
		// button on panel
		if(AbuttonsPanel.rawValue()==75||AbuttonsPanel.rawValue()==40||AbuttonsPanel.rawValue()==41||
			AbuttonsPanel.rawValue()==44||AbuttonsPanel.rawValue()==25 ) keyButton.pressDown();				
		else keyButton.pressUp();
		// key swith
		if(AbuttonsPanel.rawValue()==87||AbuttonsPanel.rawValue()==40||AbuttonsPanel.rawValue()==50||		
		AbuttonsPanel.rawValue()==25||AbuttonsPanel.rawValue()==52)		 keySwitch = true;
		else keySwitch = false;
		// key Ignition P1
		if( (AbuttonsPanel.rawValue()==104||AbuttonsPanel.rawValue()==41||AbuttonsPanel.rawValue()==50) )//&&
			keyIgnitionP1 = true;
		else keyIgnitionP1 = false;
		//(KeyIgnition1RPT < KEY_IGNITION_NUM) )	KeyIgnition1RPT++;			// filter for the input
		//else if( KeyIgnition1RPT > 0 ) KeyIgnition1RPT--;				
		//if( KeyIgnition1RPT == KEY_IGNITION_NUM) keyIgnitionP1 = true;											// change state of variable
		//else if( KeyIgnition1RPT == 0 ) keyIgnitionP1 = false;
		// key Ignition P2
		if( AbuttonsPanel.rawValue()==128||AbuttonsPanel.rawValue()==44||AbuttonsPanel.rawValue()==52 )
			keyIgnitionP2 = true;
		else keyIgnitionP2 = false;
		
		/////////////////////////////////////////////////////////////////////////
		
		
		////////////////////////////////////////////////////////////////////////
		// Buttons Logic
		if( (keyLeft.pressed||keyRigh.pressed) && (keyLeft.duration>1000 && keyLeft.duration<3200) && (keyRigh.duration>1000 && keyRigh.duration<3200) ){
			Functions(0x12);																				// blink Alert
		}
		if( !(testbit(BOOL_VAR1, BOOL_LAMP_ALERT_V1) || testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) ) ){
			if( keyLeft.pressed && (keyLeft.duration>700 && keyLeft.duration<2600) ){
				Functions(0x10);																			// blink Left
			}
			if( keyRigh.pressed && (keyRigh.duration>700 && keyRigh.duration<2600) ){
				Functions(0x11);																			// blink right
			}
		}
		if( keyMiddle.pressed ){
			//Functions(0x13);																				
		}
		// Key Panel ( P0 )
		if(AbuttonsPanel.rawValue()==255||AbuttonsPanel.rawValue()==75||AbuttonsPanel.rawValue()==87||AbuttonsPanel.rawValue()==40 ){
			if( PanelKeyRPT < PANEL_KEY_RPT_NUM )
				PanelKeyRPT ++;
			else{
				ecu.Enabled = false;
				lamp.disable();
			}
		}
		else{
			if( !testbit(BOOL_VAR1, BOOL_BAT_LOW1_V1) )
				ecu.Enabled = true;
			lamp.enable();
		}
/*		// Key Panel (P1)
		if( !keyIgnitionP1_old && keyIgnitionP1){
			keyIgnitionP1_old = true;
			if( AtripotPanel.rawValue() < 40) ecu.maxSPD = 40;
			else ecu.maxSPD = AtripotPanel.rawValue();
		}
		else if( keyIgnitionP1_old && !keyIgnitionP1)
		keyIgnitionP1_old = false;
*/
		 
		// Button Panel
		if( keyButton.pressed )
			lamp.incPWM();
		if( keySwitch ) 
			lamp.setPWM(2);
		
		
		/////////////////////////////////////////////////////////////////////////
		// inputs release
		keyLeft.release();
		keyRigh.release();
		keyMiddle.release();
		keyButton.release();
		/////////////////////////////////////////////////////////////////////////
		
		
		/////////////////////////////////////////////////////////////////////////
		// check voltage level
		if( (ArawValue[5] < (1050/VOLT_CONVERSION)) && (VoltageRPT < VOLTAGE_RPT_NUM) )
			VoltageRPT++;
		else
			VoltageRPT = 0;
		/////////////////////////////////////////////////////////////////////////
		
		
		
		if( UART_DataIsReady() ){
			char cmd = UART_Read();
			// test cmd
			if( cmd == 0x01 ){
				UART_Write(0xFE);
			}
			else if(cmd == 0x02){
				report1 = false;//T1.disable();
			}
			else if( cmd == 0x03){
				report1 = true;//T1.enable();
			}
			
			
			else if(cmd == 0x10) _timer = 0;
			else if( cmd == 0x11) _timer = 1;
			else if( cmd == 0x12){	// Set Time for blinking
				while(!UART_DataIsReady());
				timer = UART_Read();
				UART_Print("timer");
			}
			
			else if( cmd == 0x20 ){
				UART_Write(0x20);
			}
			
			else if( cmd == 0x30 ){
				MotorR.clear();
				MotorL.clear();
			}
			else if( cmd == 0x31){
				MotorR.set();
				MotorL.set();
			}
			
			else if( cmd == 0x40){
				UART_Print(IntToStr(ArawValue[0])); UART_Print("\t");
				UART_Print(IntToStr(ArawValue[6])); UART_Print("\t");
				UART_Print(IntToStr(ArawValue[7])); UART_Print("\t");
				UART_Print("\n");
			}
			
			else if( cmd == 0x50 ){
				DlightFront.clear();
				
			}
			else if( cmd == 0x51){
				DlightFront.set();
			}
			else if( cmd == 0x52){
				while(!UART_DataIsReady());
				uint8_t param1 = UART_Read();
				lamp.setPWM(param1);
			}
			else if( cmd == 0x53 ){
				if( testbit(stsLamps,0) ) clearbit(stsLamps, 0);
				else setbit(stsLamps, 0);
			}
			else if( cmd == 0x54 ){
				if( testbit(stsLamps,1) ) clearbit(stsLamps, 1);
				else setbit(stsLamps, 1);
			}
			else if( cmd == 0x55 ){
				UART_Print(IntToStr(AbuttonsPanel.rawValue())); UART_Print("\n");
			}
			else if( cmd == 0x56 ){
				UART_Print(IntToStr(keyLeft.duration)); UART_Print("\t");
				UART_Print(IntToStr(keyRigh.duration)); UART_Print("\n");
			}
			else if( cmd == 0x57 ){
				keyLeft.release();
				keyRigh.release();
			}
			else if( cmd == 0x58){
				lamp.enable();
			}
			else if( cmd == 0x58){
				
			}
			
			
			
			else if( cmd == 0x60 ){
				uint16_t volts = ArawValue[5];
				volts *= VOLT_CONVERSION;
				UART_Print("\n");
				UART_Print(IntToStr(volts));
				UART_Print("\n");
			}
			else if( cmd == 0x61){
				Functions(0x10);
			}
			else if( cmd == 0x62 ){
				Functions(0x11);
			}
			else if( cmd == 0x63 ){
				 UART_Print(IntToStr(keyIgnitionP1)); UART_Print("\n");
				 UART_Print(IntToStr(keyIgnitionP2)); UART_Print("\n");
			}
			
			else if( cmd == 0x70){
				while(!UART_DataIsReady());
				uint8_t value = UART_Read();
				UART_Print(IntToStr(Athrottle.calcExp(value)));
				UART_Print("\n");
			}
			else if( cmd == 0x71){
				 UART_Print(IntToStr(Athrottle.getMinRange())); UART_Print(", ");
				 UART_Print(IntToStr(Athrottle.getMaxRange())); UART_Print(", ");
				 UART_Print(IntToStr(Athrottle.Value())); UART_Print(", ");
				 UART_Print("\n");
			}
			
			
		}
    }
}