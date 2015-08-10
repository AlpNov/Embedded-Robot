/*
 * usr_uart.h
 *
 *  Created on: Oct 4, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#ifndef USR_UART_H_
#define USR_UART_H_

#define UART_BASE UART3_BASE
#define UART_BDRT 115200


void InitUART3();
void InitUART0();
void UARTPutString(unsigned char *pucBuffer);
void UARTGetString(unsigned char *pucBuffer, unsigned long ulCount);

#endif /* USR_UART_H_ */
