/*
 * University of Houston
 * ECE 4437 - Embedded Microcomputer Systems
 * Fall 2013
 * Team 14 - An Nguyen
 * 			 Paolina Povolotskaya
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "usr_include/usr_uart.h" 	// Switch between USB_UART(0) and BT_UART(3)
#include "usr_include/modbus.h" 	// modbus functions
#include "usr_include/cmd_interp.h" // function interpreter
#include "usr_include/motor.h"		// Motors
#include "usr_include/line.h"		// Line Sensor
#include "usr_include/ultrasonic.h"	// Ultrasonic Sensor
#include "usr_include/IRdist.h"		// IR distance sensor
#include "usr_include/color.h"		// color sensor
#include "usr_include/functions.h"
#include "usr_include/shaft_encoder.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//	Temp Space for functions

//*****************************************************************************

//*****************************************************************************
// Start Main
//*****************************************************************************
int main(void) {
	char data_in[128];

  //***************************************************************************
  // Setting the system Clock
  //
  // 400 MHz PLL driven by 16 MHz external crystal
  // builtin /2 from PLL, then /4 --> 50Hz Clock speed
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  //
  //***************************************************************************

	InitUART3();
	MotorInit();		//Initialize Motors
	LineInit();			//Initialize Line Sensors
	ultrasonicInit();	//Initialize UltraSonic
	IRdsitinit();		//Initialize IR Distance Sensor
	colorinit();		//Initialize Color Sensor
	FollowINIT();		//Initialize timer interrupts for following
	ShaftEncoder_Init();
	IntMasterEnable();	//Enable Interrupts
	UARTprintf("An & Paolina's Robot says Hello\n\r");



	while (1) {
		UARTgets(data_in,128);
		MB_Get((unsigned char *)data_in);
		Interpret(data_in);

	}


}
