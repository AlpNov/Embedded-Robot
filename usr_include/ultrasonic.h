/*
 * ultrasonic.h
 *
 *  Created on: Oct 15, 2013
 *      Author: ppovolot
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_
// Initialize the Ultrasonic Sensor SysTick Timer
void ultrasonicInit(void);
// Obtain the Raw Timer Value (50 MHz Clock)
unsigned long ultrasonic(void);

void Timer5AHandler(void);

#endif /* ULTRASONIC_H_ */
