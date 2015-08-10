/*
 * shaft_encoder.h
 *
 *  Created on: Nov 9, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#ifndef SHAFT_ENCODER_H_
#define SHAFT_ENCODER_H_

void ShaftEncoder_Init(void);
void ShaftEncoder_Clear(void);
unsigned long ShaftEncoder_Get(void);
void Timer4AHandler(void);

void GPIO_PortF_IntHandler(void);


#endif /* SHAFT_ENCODER_H_ */
