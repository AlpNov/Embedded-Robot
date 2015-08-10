/*
 * motor.h
 *
 *  Created on: Oct 5, 2013
 *      Author: annnguyen
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define FREQ 500
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define COAST 0
#define LEFTMOTOR 1
#define RIGHTMOTOR 2
#define BOTHMOTOR 0

void MotorInit();
void Motor(int motor,int speed,int direction);

int GetMotorDirection(int);
int GetMotorSpeed(int);

void Timer0IntHandler(void);
void Timer1IntHandler(void);

#endif /* MOTOR_H_ */
