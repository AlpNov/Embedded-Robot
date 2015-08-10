/*
 * line.c
 *
 *  Created on: Oct 5, 2013
 *      Author: Paolina Povolotskaya
 *      Co-Author: An Nguyen
 */

#include "inc/lm4f120h5qr.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "modbus.h"
#include "driverlib/uart.h"
#include "usr_uart.h"
#include "driverlib/uart.h"
#include "line.h"

/**********************************************************************
* Function: void line_Init(void)
* Input:  none
* Output: none
*
* Purpose: Initializes I/O port pins for the line sensors
************************************************************************/
void LineInit(void) {
	    // configure the clock for 50 MHz
		//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	    // Enable GPIO port E
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    // PE1 (sensor 1 - left)
	    // PE2 (sensor 2 - right)
	    // Set the direction as output and enable for digital function
		GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);

}


unsigned int lineSensor(char Sensor)
{
	if(Sensor == RIGHT)               //right sensor PE2
  {
	// set PE2 to output
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);
	// output high to charge the capacitor to 5v
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
	// wait until capacitor charges, 1 ms
	SysCtlDelay(SysCtlClockGet()/(10000*3));
	// set PE2 to input
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
	// wait for capacitor to discharge, 0.5ms
	SysCtlDelay(SysCtlClockGet()/(2000*3));
	// return value read
    if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2))
    	return BLACK;
    else
    	return WHITE;
  }
  else if(Sensor == LEFT)					//left sensor
  {
	  // set PE1 to output
	  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
	  // output high to charge the capacitor to 5v
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);
	  // wait until capacitor charges, 1ms
	  SysCtlDelay(SysCtlClockGet()/(10000*3));
	  // set PE1 to input
	  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
	  // wait for capacitor to discharge, 1ms
	  SysCtlDelay(SysCtlClockGet()/(1000*3));
	  // return value read
	  if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1))
		  return BLACK;
	  else
		  return WHITE;
  }
  else
  {
    return 2;                //default returns no line (white)
  }
}
