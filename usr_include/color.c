/*
 * color.c
 *
 *  Created on: Oct 18, 2013
 *      Author: annnguyen
 */
//*******************************************************************************************
// Color Filter:	S2	S3				S2= PB0
//					L	L	Red			S3= PB1
//					L	H	Blue
//					H	L	Clear
//					H	H	Green
//
//	OUT (F0) = PA5
//********************************************************************************************
#include "inc/lm4f120h5qr.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ints.h"
#include "color.h"

#define WAIT_FREQ 10
static int color;
static unsigned long red_counter,blue_counter,green_counter, clear_counter;

// Initialize the color sensor and the Interrupt handler for PA5
void colorinit(void){
	// Enable GPIO port A and B
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// PB0 : S2
	// PB1 : S3
	// PB5 : Frequency OUT
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
	// PA5 : OE, turn High to disable Chip
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);

	// PB3, PC4, PC5: RGB LED
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_4 | GPIO_PIN_5);

	//Enable Port B5 Falling Edge Interrupt
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTB_BASE,GPIO_PIN_5);


}

// Test Red Filter
// Return raw value of falling edge interrupts for 250 ms interval
int Red(void){

	color = RED;
	//Turn On LED
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
	//Turn OE Low
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
	// Set S2 and S3 to Low
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 , 0x00);
	// Set OE to Low
	// Reset Interrupt Counter
	red_counter = 0;

	//Enable GPIO Port A interrupt
	IntEnable(INT_GPIOB);

	// Wait 250 ms
	SysCtlDelay(SysCtlClockGet()/(WAIT_FREQ*3));
	// Turn OE High
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
	IntDisable(INT_GPIOB);
	//Turn off LED
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);
	return red_counter;

}

// Test Blue Filter
// Return raw value of falling edge interrupts for 250 ms interval
int Blue(void){

	color = BLUE;
	//Turn on LED
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
	//Turn OE Low
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
	// Set S2 to low and S3 to high
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 , 0x00);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1 , GPIO_PIN_1);
	// Reset Interrupt Counter
	blue_counter = 0;

	//Enable GPIO Port A interrupt
	IntEnable(INT_GPIOB);

	// Wait 250 ms
	SysCtlDelay(SysCtlClockGet()/(WAIT_FREQ*3));
	// Turn OE High
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
//	IntDisable(INT_GPIOB);
	//Turn off LED
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5);
	return blue_counter;

}

// Test Green Filter
// Return raw value of falling edge interrupts for 250 ms interval
int Green(void){

	color = GREEN;
	//Turn on LED
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
	//Turn OE Low
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
	// Set S2 and S3 to high
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 , GPIO_PIN_0 | GPIO_PIN_1);
	// Reset Interrupt Counter
	green_counter = 0;

	//Enable GPIO Port A interrupt
	IntEnable(INT_GPIOB);

	// Wait 250 ms
	SysCtlDelay(SysCtlClockGet()/(WAIT_FREQ*3));
	// Turn OE High
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
//	IntDisable(INT_GPIOB);
	//Turn off LED
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
	return green_counter;

}

// Test Clear Filter
// Return raw value of falling edge interrupts for 250 ms interval
int Clear(void){

	color = CLEAR;
	//Turn on LED
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
	//Turn OE Low
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
	// Set S2 and S3 to high
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 , GPIO_PIN_0 | GPIO_PIN_1);
	//Enable Interrupt Counter
	clear_counter = 0;

	//Enable GPIO Port A interrupt
	IntEnable(INT_GPIOB);

	// Wait 250 ms
	SysCtlDelay(SysCtlClockGet()/(WAIT_FREQ*3));
	// Turn OE High
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
//	IntDisable(INT_GPIOB);
	//Turn off LED
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);
	return clear_counter;

}

// Increments interrupt counters for each filter
void GPIO_PORTB_IntHandler (void) {
	//Clear and Disable Interrupt
	GPIOPinIntClear(GPIO_PORTB_BASE,GPIO_PIN_5);
	GPIOPinIntDisable(GPIO_PORTB_BASE,GPIO_PIN_5);
	// Count Interrupts for each color
	if(color == RED)
		red_counter++;
	if(color == BLUE)
		blue_counter++;
	if(color == GREEN)
		green_counter++;
	if(color == CLEAR)
		clear_counter++;
	//Enable Interrupt
	GPIOPinIntEnable(GPIO_PORTB_BASE,GPIO_PIN_5);

}

int GetColor(void){
	int r, g, b, RG, RB, GB, color;

	// get colors
	r=Red();
	b=Blue();
	g=Green();
	// calculate ratios
	RG = r*100/g;
	RB = r*100/b;
	GB = g*100/b;
	UARTprintf("r,g,b - RG, RB, GB: %2u, %2u, %2u \r\n", RG, RB, GB);

	if ( (RB > 300) || (RB < 60) || (RG > 600) || (RG < 250) )
		color = UNKNOWN_COLOR;
	else if (RG < 325){
		if (RB > 170)
			color = YELLOW;
		else if (RB > 140)
			color = GREEN;
		else
			color = BLUE;
	}
	else{
		if (RB > 160)
			color = RED;
//		if (RB > 200)
//			color = YELLOW;
//		else if (RB > 160)
//			 color = RED;
//		 else if (RB > 130)
//			 color = GREEN;
//		 else if (RB > 90)
//			 color = BLUE;
//		 else
		color = UNKNOWN_COLOR;
	}
//	if ((RB < 100) || (RG < 100)) color = UNKNOWN_COLOR;
//	else if(RB < 360 && RG < 275) color = BLUE;
//	else if(RB > 360 && RB < 400 && RG< 275) color = GREEN;
//	else if(RB > 410 && RG < 275) color = YELLOW;
//	else if(RB > 320 && RG > 275) color = RED;
//	else color = UNKNOWN_COLOR;
	return color;
}
