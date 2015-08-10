/*
 * usr_uart.h
 *
 *  Created on: Oct 4, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "usr_uart.h"
#include "modbus.h"


#define MB_ADDRESS 'k'

//*****************************************************************************
// Byte2Hex
// Convert 00010001 to 0x11
//*****************************************************************************

//*****************************************************************************
// Defining Modbus address
//*****************************************************************************
#define MB_ADDRESS 'k'
#define MB_GET_BUFFER 80
//*****************************************************************************
// MB_Get - Receive Modbus Frame and extract data
// return 0 - success
// 		  1 - Wrong starting byte, not a modbus package
//		  2 - Wrong address
//	 	  3 - Failed Check sum
//*****************************************************************************
int MB_Get(unsigned char *data_string){
	unsigned char checksum;
	unsigned long charcount = 2;
	unsigned long data_length = 0;
	//
	// Check for Modbus starting byte '~'
	//
	if (data_string[0] != '~') {
		return 1;
	}
	checksum = '~';
	//
	// Check for address
	if (data_string[1] != MB_ADDRESS) {
		return 2;
	}
	checksum = checksum ^ MB_ADDRESS;
	//Get length of data, including checksum char
	while (data_string[charcount] != 0) {
		data_string[data_length++] = data_string[charcount++];
	}
	//Check checksum, if zero -> correct
	//Use dummy checksum '5'
	checksum = '5';
	if ((data_string[--data_length] ^ checksum) != 0) {
		return 3;
	}
	//replace checksum with NULL
	data_string[data_length] = 0;

	//
	//Shift data to the front, discarding start byte, address byte and checksum
	//

    return 0;
}

//*****************************************************************************
// MB_Put - Wrap a data string with modbus overhead and send
//*****************************************************************************
void MB_Put(const unsigned char *data_string) {
	int length = 0;
	int charcount = 0;
	char frame[128];
	unsigned char checksum;
	//
	//Find length of data
	//
	while (*(data_string + length++) != '\0');
	//
	//Modbus Header
	//
	frame[charcount] = '~'; //starting bit is a ":"
	checksum =  0 ^ frame[charcount++];
	frame[charcount] =  MB_ADDRESS;
	checksum = checksum ^ frame[charcount++];
	//
	//Data
	//
	while (length--) {
		frame[charcount] = data_string[charcount - 2];
		checksum = checksum ^ frame[charcount++];
	}
	//
	//Modbus Footer
	//
	frame[charcount++] = checksum;
	frame[charcount++] =  13;
	frame[charcount++] =  10;
	frame[charcount] = 0;
	//
	//Send
	//
	UARTprintf("%s\r\n",frame);
}
