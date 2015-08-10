/*
 * color.h
 *
 *  Created on: Oct 18, 2013
 *      Author: annnguyen
 */

#ifndef COLOR_H_
#define COLOR_H_

#define UNKNOWN_COLOR 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define CLEAR 5

void colorinit(void); // Initialize Color Sensor and Falling Edge Interrupts for PA5
int Red(void);		  // Test Red Filter
int Blue(void);		  // Test Blue Filter
int Green(void);	  // Test Green Filter
int Clear(void);	  // Test Clear Filter
int GetColor(void);	  // Gets the Color [1-Red, 2-Green, 3-Blue, 4-Yellow, 0- none]

// Interrupt Handler for Port A
void GPIO_PORTA_IntHandler (void);


#endif /* COLOR_H_ */
