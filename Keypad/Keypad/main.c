/*
 * Keypad.c
 *
 * Created: 09.02.2020 15:40:44
 * Author : Marc
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"
#include <stdio.h>
#include <string.h>

typedef struct  
{
	volatile uint8_t * const DDRx;
	volatile uint8_t * const PORTx;
	volatile uint8_t * const PINx;
	const uint8_t pinnumber;
} GenericPinDescriptor_t;

const GenericPinDescriptor_t R2 = {&DDRC,&PORTC,&PINC,0U};
const GenericPinDescriptor_t R1 = {&DDRC,&PORTC,&PINC,1U};
const GenericPinDescriptor_t R3 = {&DDRC,&PORTC,&PINC,2U};
const GenericPinDescriptor_t R4 = {&DDRC,&PORTC,&PINC,3U};
const GenericPinDescriptor_t R5 = {&DDRC,&PORTC,&PINC,4U};
const GenericPinDescriptor_t C5 = {&DDRC,&PORTC,&PINC,5U};
const GenericPinDescriptor_t C4 = {&DDRC,&PORTC,&PINC,6U};
const GenericPinDescriptor_t C3 = {&DDRC,&PORTC,&PINC,7U};
const GenericPinDescriptor_t C2 = {&DDRA,&PORTA,&PINA,7U};
const GenericPinDescriptor_t C1 = {&DDRA,&PORTA,&PINA,6U};

#define NUM_OF_ROWPINS (5U)
#define NUM_OF_COLPINS (5U)

const GenericPinDescriptor_t * const rowpins[NUM_OF_ROWPINS]={&R2,&R1,&R3,&R4,&R5};
const GenericPinDescriptor_t * const colpins[NUM_OF_COLPINS]={&C5,&C4,&C3,&C2,&C1};

const char keyMatrixIndicesToCharLUT[NUM_OF_ROWPINS][NUM_OF_COLPINS] = 
	{	//c0 c1  c2  c3  c4 
		{'\r','\a','6','5','4'}, //R0
		{'\b','\a','3','2','1'}, //R1
		{'\a','\a','9','8','7'}, //R2
		{'\a','\n','#','0','*'}, //R3
		{'\a','\a','\a','\a','\a'}, //R4
	};

	
__attribute__((constructor)) void mainInit(void)
{
	for (uint8_t row=0;row<NUM_OF_ROWPINS;row++)
		*(rowpins[row]->DDRx) |= 1<<(rowpins[row]->pinnumber);
	USART0_init(USARTBaudRate_9600);
	sei();
}

#pragma GCC diagnostic ignored "-Wmain"
__attribute__((noreturn)) void main(void)
{
    /* Replace with your application code */
    while (1) 
    {	
		for(uint8_t col=0;col<NUM_OF_COLPINS;col++) 
		{
			*(colpins[col]->PORTx) |= 1<<colpins[col]->pinnumber; // one colpin as input w/ pullups 
			for (uint8_t row=0;row<NUM_OF_ROWPINS;row++)
			{
				*(rowpins[row]->PORTx) &= ~(1<<(rowpins[row]->pinnumber));
				if(!(*(colpins[col]->PINx) & (1<<colpins[col]->pinnumber))) // if that pin is low there it is connected to some row pin that drives it low
				{
					USART0_SendChar(keyMatrixIndicesToCharLUT[row][col]);
				}
				*(rowpins[row]->PORTx) |= (1<<rowpins[row]->pinnumber);
				_delay_ms(7);
			}
			*(colpins[col]->PORTx) &= ~(1<<colpins[col]->pinnumber); // tristate the pin
		}
    }
}

