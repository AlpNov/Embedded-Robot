/*
 * motor.c
 *
 *  Created on: Oct 5, 2013
 *      Author: An Nguyen
 *   Co-Author: Paolina Povolotskaya
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
#include "motor.h"


//*****************************************************************************
//	Local Variables
//*****************************************************************************
static unsigned long pwm_uptime_left,pwm_uptime_right;
static unsigned long pwm_period;
static unsigned int left_direction,right_direction;


void MotorInit(void){
	//Set motor Pin as output for 1A,2A,3A and 4A, 12EN and 34EN
	//1A = PD0, 2A = PD1, 3A = PD2, 4A = PD3
	//12EN = PF2, 34EN = PF3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);

	//Calculate period and Set Motor speed to 0
	pwm_period = ((SysCtlClockGet() / FREQ) / 2);
	Motor(BOTHMOTOR,0,BRAKE);


	//Initialize Timer 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, pwm_period);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);

	//Initialize Timer 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER1_BASE, TIMER_A, pwm_period);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER1_BASE, TIMER_A);
}


static void LeftForward() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1,GPIO_PIN_0);
	left_direction=FORWARD;
}
static void LeftBackward() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1,GPIO_PIN_1);
	left_direction=BACKWARD;
}
static void LeftBrake() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1,GPIO_PIN_0|GPIO_PIN_1);
	left_direction=BRAKE;
}

static void RightForward() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_2);
	right_direction=FORWARD;
}
static void RightBackward() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_3);
	right_direction=BACKWARD;
}
static void RightBrake() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_2|GPIO_PIN_3);
	right_direction=BRAKE;
}

int GetMotorDirection(int motor) {
	return  ( (motor == RIGHTMOTOR) ? right_direction : left_direction );
}
int GetMotorSpeed(int motor) {
	return  ( (motor == RIGHTMOTOR) ? (pwm_uptime_right*100/pwm_period) : (pwm_uptime_left*100/pwm_period) );
}
void Motor(int motor,int speed,int direction) {
	//Left  1A = PD0, 2A = PD1
	//Right 3A = PD2, 4A = PD3
	//12EN = PF2, 34EN = PF3
	if (motor == LEFTMOTOR) {
		pwm_uptime_left = (pwm_period*speed)/100;
		if (direction == FORWARD)
			LeftForward();
		else if (direction == BACKWARD)
			LeftBackward();
		else if (direction == BRAKE)
			LeftBrake();
	}
	else if (motor == RIGHTMOTOR) {
		pwm_uptime_right = (pwm_period*speed)/100;
		if (direction == FORWARD)
			RightForward();
		else if (direction == BACKWARD)
			RightBackward();
		else if (direction == BRAKE)
			RightBrake();
	}
	else if (motor == BOTHMOTOR) {
		pwm_uptime_right = pwm_uptime_left = (pwm_period*speed)/100;
		if (direction == FORWARD) {
			LeftForward();
			RightForward();
		}
		else if (direction == BACKWARD) {
			RightBackward();
			LeftBackward();
		}
		else if (direction == BRAKE) {
			RightBrake();
			LeftBrake();
		}
	}
	pwm_uptime_left = pwm_uptime_left*90/100;
}

// Left PWM interrupt handler
void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Read the current state of the GPIO pin and
	// write back the opposite state
	//12EN = PF2, 34EN = PF3
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
		TimerLoadSet(TIMER0_BASE, TIMER_A, pwm_period - pwm_uptime_left);
	}
	else
	{
		//Only set pin to high and change timer period if uptime != 0
		if (pwm_uptime_left) {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
			TimerLoadSet(TIMER0_BASE, TIMER_A, pwm_uptime_left);
		}
	}
}

// Right PWM interrupt handler
void Timer1IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);


	// Read the current state of the GPIO pin and
	// write back the opposite state
	//12EN = PF2, 34EN = PF3
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3))
	{
		GPIOPinWrite(GPIO_PORTF_BASE , GPIO_PIN_3, 0);
		TimerLoadSet(TIMER1_BASE, TIMER_A, pwm_period - pwm_uptime_right);
	}
	else
	{
		//Only set pin to high and change timer period if uptime != 0
		if (pwm_uptime_right) {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
			TimerLoadSet(TIMER1_BASE, TIMER_A, pwm_uptime_right);
		}
	}
}
