/*
 * IRdist.c
 *
 *  Created on: Oct 10, 2013
 *      Author: ppovolot
 */

#include "inc/lm4f120h5qr.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "modbus.h"
#include "driverlib/uart.h"
#include "usr_uart.h"
#include "driverlib/uart.h"

void IRdsitinit(void){
		// configure the clock for 50 MHz
		SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
		// Enable the ADC0 peripheral
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
		// Enable GPIO port E
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
		// Set the pin for analog ADC function
		GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 );
		// Enable for a single sample when the processor sends a signal to start
		ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
		 // Sample channel 0 (ADC_CTL_CH0) in
		 // single-ended mode (default) and configure the interrupt flag
		 // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
		 // that this is the last conversion on sequence 3 (ADC_CTL_END).
		 ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
		                             ADC_CTL_END);
		 // Enable sample sequence 3
		 ADCSequenceEnable(ADC0_BASE, 3);

}

unsigned long IRdist(void){
		unsigned long dist[1];

		// Clear the interrupt status flag.
		ADCIntClear(ADC0_BASE, 3);
		// Trigger the ADC conversion.
		ADCProcessorTrigger(ADC0_BASE, 3);
		// Wait for conversion to be completed.
		while(!ADCIntStatus(ADC0_BASE, 3, false))
		{
		}
		// Clear the ADC interrupt flag.
		ADCIntClear(ADC0_BASE, 3);
		// Read ADC Value.
		ADCSequenceDataGet(ADC0_BASE, 3, dist);
		// Return the value
		return dist[0];

}
