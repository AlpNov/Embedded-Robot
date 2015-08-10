/*
 * shaft_encoder.c
 *
 *  Created on: Nov 9, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#include "inc/lm4f120h5qr.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ints.h"
#include "shaft_encoder.h"

#define ENCODER_MAX_FREQ 40

static unsigned long Encoder_Count;

void ShaftEncoder_Init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

/*
	GPIOPortIntRegister(GPIO_PORTF_BASE,&GPIO_PortF_IntHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
	IntEnable(INT_GPIOA);*/

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
	TimerConfigure(TIMER4_BASE, TIMER_CFG_A_PERIODIC);
	TimerLoadSet(TIMER4_BASE, TIMER_A, SysCtlClockGet()/(ENCODER_MAX_FREQ*3));
	TimerIntRegister(TIMER4_BASE, TIMER_A, &Timer4AHandler);
	IntEnable(INT_TIMER4A);
	TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER4_BASE, TIMER_A);

	Encoder_Count = 0;
}
void ShaftEncoder_Clear(void) {
	Encoder_Count = 0;
}
unsigned long ShaftEncoder_Get(void) {
	return Encoder_Count;
}

void GPIO_PortF_IntHandler(void) {
	GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPinIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
	Encoder_Count++;
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);

}

void Timer4AHandler(void) {
	static int prev = 0, prev_2 = 0;
	TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	if ( (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == prev) && (prev != prev_2)) {
		Encoder_Count++;
	}
	prev_2=prev;
	prev = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
}
