/*
 * usr_uart.h
 *
 *  Created on: Oct 4, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
 */

#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

void Interpret(char*);
void Hello();
void Print_Help();


//****************************************************************************
// Competition Functions
void ToCanTest(void);
void DetectCanTest(void);

//****************************************************************************
// Left Turn Test Functions
void SharpTurnTest();
void SmoothTurnTest();
void PanTest();

//****************************************************************************
// Motor Control functions
void Forward();
void Backward();
void LeftForward();
void RightBackward();
void Brake();
void Coast();
void Slower();
void Faster();
void TimedForward();

//****************************************************************************
// Shaft Encoder Function
void PrintEncoder();
void TurnLeft();
void TurnRight();


//****************************************************************************
// Wall Follow function
void WallTest(void);
void StopTest(void);

//****************************************************************************
// line Test function
void FollowTest(void);


//****************************************************************************
// IR Sensor Test Function
void IRTest(void);

//****************************************************************************
// Color Sensor Test Function
void ColorTest(void);
void PrintColor(void);


//****************************************************************************
// Line Sensor Test Functions
void Line_left(void);
void Line_right(void);
//****************************************************************************

//****************************************************************************
// Ultrasonic Test Function
void PrintUltrasonic();
//****************************************************************************

void Invalid(char *);

#endif /* CMD_INTERP_H_ */
