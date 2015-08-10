/*
 * modbus.h
 *
 *  Created on: Oct 4, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#ifndef MODBUS_H_
#define MODBUS_H_

//Get Modbus frame, extract data, return int code for errors/success
int MB_Get(unsigned char *data_string);
//Take character pointer/string, wrap inside modbus and send through to UART
void MB_Put(const unsigned char *data_string);
//Send string to UART



#endif /* MODBUS_H_ */
