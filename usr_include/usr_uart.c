/*
 * usr_uart.c
 *
 *  Created on: Oct 4, 2013
 *      Author: annnguyen
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "usr_uart.h"


void InitUART3() {
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    UARTStdioInit(3);
}

void InitUART0() {
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTStdioInit(0);
}

//*****************************************************************************
// Get a string until Enter (\c\r) is encountered or the max length is reached
// Credit: uart_echo.c - Stellarisware
//*****************************************************************************
void UARTGetString(unsigned char *pucBuffer, unsigned long ulCount){
	unsigned long charcount = 0;
	while (charcount <= ulCount) {
		pucBuffer[charcount] = UARTCharGet(UART_BASE);
		if ((pucBuffer[charcount] == '\r') || (pucBuffer[charcount] == '\n') )
			break;
		charcount++;
	}
	pucBuffer[charcount] = 0;
}

//*****************************************************************************
// Send a string to the UART.
// Credit: uart_echo.c - Stellarisware
//*****************************************************************************
void UARTPutString(unsigned char *pucBuffer)
{
    //
    // Loop while there are more characters to send.
    //
    while(*pucBuffer != 0)
    {
        //
        // Write the next character to the UART.
        //
    	UARTCharPut(UART_BASE, *pucBuffer++);
    }
}
