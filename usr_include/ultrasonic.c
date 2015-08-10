/*
 * ultrasonic.c
 *
 *  Created on: Oct 15, 2013
 *      Author: ppovolot
 */

#include "inc/lm4f120h5qr.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "modbus.h"
#include "usr_uart.h"
#include "ultrasonic.h"


	static unsigned long count = 0;

// Ultrasonic Sensor and SysTick timer Initialization
void ultrasonicInit(void){
	// Enable GPIO port E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// PE4 : Trigger Output
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
	// Output Trigger Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
	// PE5 : Echo Input
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_5);

	// Configure to SysTick Timer wrap every 150ms
	SysTickPeriodSet(SysCtlClockGet()/(1000/150));
	//Enable the SysTick counter
	SysTickEnable();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
	TimerConfigure(TIMER5_BASE, TIMER_CFG_A_PERIODIC);
	TimerLoadSet(TIMER5_BASE, TIMER_A, SysCtlClockGet()/50);
	TimerIntRegister(TIMER5_BASE, TIMER_A, &Timer5AHandler);
	IntEnable(INT_TIMER5A);
	TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER5_BASE, TIMER_A);
}

unsigned long ultrasonic(void){

	return count;
}

void Timer5AHandler(void){

	TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

	// Output Trigger Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
	// Wait 2 microseconds
	SysCtlDelay(SysCtlClockGet()/(500000*3));
	// Output Trigger High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
	// Wait for 10 microseconds
	SysCtlDelay(SysCtlClockGet()/(100000*3));
	// Output Trigger Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
	//Wait for Echo Pin to go High
	while(!GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5));
	//Reset systick
	HWREG(NVIC_ST_CURRENT) = 0x00000000;
	// Wait for Echo to be set to Low
	while(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5));
	// Read timer value
	count = SysCtlClockGet()/(1000/150) - SysTickValueGet();

}
