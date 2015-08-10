/*
 * line.h
 *
 *      Author: Paolina Povolotskaya
 *      Co-Author: An Nguyen
 *
*/

#ifndef LINE_H_
#define LINE_H_

// Takes in the value of which line sensor to read 0- left or 1- right
// Returns the value of the line sensor read
unsigned int lineSensor(char Sensor);
// Initializes the left and right line sensors
void LineInit(void);

#define BLACK 1
#define WHITE 0
#define LEFT 1
#define RIGHT 0

#endif /* LINE_H_ */
