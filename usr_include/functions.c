/*
 * functions.c
 *
 *  Created on: Nov 2, 2013
 *      Author: Fairy_000
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//#include "../utils/uartstdlib.h"
#include "ultrasonic.h"
#include "motor.h"
#include "line.h"
#include "IRdist.h"
#include "color.h"
#include "functions.h"
#include "shaft_encoder.h"

#define REF 19800
#define TIME SysCtlClockGet()/50
#define MAXSPEED 100
#define MINSPEED 50
#define MAXERR REF
#define	NOWALL 38000

signed int error, rate, integrator, previous;
int ctl, currentRoom, can, Stop;

//Hard code variables
static unsigned int fakeit = 0;
static int fake_color[4];



void FollowINIT(void){
	int period = TIME;
	//Initialize Timer 3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER3_BASE, TIMER_A, period);
	ctl = NOTHING;
	IntEnable(INT_TIMER3A);
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER3_BASE, TIMER_A);

}

//****************************************************************************
// Detect Can Color
int DetectCan(void){
	unsigned long value=0,IRSample;
	int color, i=0;

	// Go into the Room
//	Motor(BOTHMOTOR,50,FORWARD);
//	ShaftEncoder_Clear();
//	while(ShaftEncoder_Get()<3);
	// Pan Left
	PanLeft();

	// Turn Right until distance increases
	Motor(RIGHTMOTOR, 50, BACKWARD);
	Motor(LEFTMOTOR, 50, FORWARD);

	//Get 125 samples, 5 ms apart
	while(i<125){
		IRSample = IRdist();
		if(value < IRSample){
			value = IRSample;
		}
		SysCtlDelay(SysCtlClockGet()/(200*3));
		UARTprintf("Distance Sample #%u: %u, %u\r\n",i, IRSample, value);
		i++;
	}

	value = value*98/100; // 99% of maximum value
	UARTprintf("Set Max Value at %u\r\n",value);

	//Turn left until value is larger than scaled maximum
	Motor(RIGHTMOTOR, 40, FORWARD);
	Motor(LEFTMOTOR, 40, BACKWARD);

	i = 0;
	IRSample = IRdist();
	while (IRSample < value) {
		IRSample = IRdist();
		UARTprintf("%u < %u\r\n",IRSample, value);
		SysCtlDelay(SysCtlClockGet()/(1000*3));
	}
	Motor(BOTHMOTOR, 100, BRAKE);
	//UARTprintf("%u > %u\r\n",IRSample, value);

	// Go forward until close to the can
	Motor(BOTHMOTOR, 50, FORWARD);
	i=0;

	value = 2600;
	while(value >= IRdist()){// Go forward
		Motor(BOTHMOTOR, 50, FORWARD);
		// for 50 ms
		SysCtlDelay(SysCtlClockGet()/(20*3));
	}
	// Stop Motors
	Motor(BOTHMOTOR, 0, BRAKE);

	// Get the Color
	color = GetColor();

	while(color == UNKNOWN_COLOR){ // Get values until the color is identified
		ShaftEncoder_Clear();
		Motor(BOTHMOTOR, 40, FORWARD);
		while (ShaftEncoder_Get() < 1);
		Motor(BOTHMOTOR, 100, BRAKE);
		color=GetColor();
	}

	if(color == RED) UARTprintf("RED\n");
	else if(color == GREEN) UARTprintf("GREEN\n");
	else if(color == BLUE) UARTprintf("BLUE\n");
	else if(color == YELLOW) UARTprintf("YELLOW\n");

	return color;

} //End DetectCan
//****************************************************************************

void FollowToCan(void){
	// Follow line until both sensors see black
	while ( (lineSensor(LEFT) == WHITE ) || (lineSensor(RIGHT) == WHITE ) ){
		ctl=LINE;
	}
	ctl=NOTHING;
	Motor(BOTHMOTOR, 100, BRAKE);
	// Orient and go into room to detect color
	do {
		if (!fakeit){
			ShaftEncoder_Clear();
			Motor(BOTHMOTOR, 40, FORWARD);
			while (ShaftEncoder_Get() < 1);
			Motor(BOTHMOTOR, 100, BRAKE);
			can = GetColor();
		}
		else {
			UARTprintf("Current room %d \n\r",currentRoom);
			can = fake_color[can-1];
		}
	} while(can == UNKNOWN_COLOR);
	if(can == RED) UARTprintf("RED\n");
	else if(can == GREEN) UARTprintf("GREEN\n");
	else if(can == BLUE) UARTprintf("BLUE\n");
	else if(can == YELLOW) UARTprintf("YELLOW\n");
	// Turn Left
	Motor(LEFTMOTOR, 60, BACKWARD);
	Motor(RIGHTMOTOR, 60, FORWARD);
	ShaftEncoder_Clear();
	while(ShaftEncoder_Get()<6);
	// Until right sensor detects the black line
//	while(lineSensor(RIGHT) != BLACK);
	// Until left sensor detects the black line
	while(lineSensor(LEFT) != BLACK);
	Motor(BOTHMOTOR,100,BRAKE);
}

void FollowRight(void){ // Follow Right Edge with Right Sensor

	if (lineSensor(RIGHT) == BLACK) // See Black
				{	 // Turn Right
				 	 Motor(LEFTMOTOR, MAXSPEED, FORWARD);
				 	 Motor(RIGHTMOTOR, 30, FORWARD);
				}

	if (lineSensor(RIGHT) == WHITE) // See white
				{	 // Turn Left
					Motor(LEFTMOTOR, 30, FORWARD);
					Motor(RIGHTMOTOR, MAXSPEED, FORWARD);
				}
}

void FollowLeft(void){// Follow Left edge with left sensor

	static int lineleft;
	lineleft = lineSensor(LEFT);
	if (lineleft == WHITE)
	{	 // Turn right
		Motor(LEFTMOTOR, MAXSPEED, FORWARD);
		Motor(RIGHTMOTOR, MINSPEED , FORWARD);
	}
	else if (lineleft == BLACK)
	{	 // Turn Left
		Motor(LEFTMOTOR, MINSPEED, FORWARD);
		Motor(RIGHTMOTOR, MAXSPEED, FORWARD);
	}
}

void FollowWall(void){
	signed int kp, ki, kd, angle, LeftSpeed,RightSpeed;
	kp=1;
	ki=0;
	kd=0;
	angle = kp*error + kd*rate + ki*integrator;

	// angle > 0 : Robot is further to the wall than REF
	if(angle > 0){
		LeftSpeed = MAXSPEED - ((MAXSPEED - MINSPEED)*angle)/(MAXERR/2);
		LeftSpeed = LeftSpeed < MINSPEED ? MINSPEED : LeftSpeed;
		RightSpeed = MAXSPEED;
		Motor(RIGHTMOTOR, RightSpeed, FORWARD); // Turn to Wall
		Motor(LEFTMOTOR, LeftSpeed, FORWARD);

	}
	// angle < 0 : Robot is closer from wall then REF
	if(angle < 0){
		RightSpeed = MAXSPEED + ((MAXSPEED - MINSPEED)*angle)/(MAXERR/4);
		RightSpeed = RightSpeed < MINSPEED ? MINSPEED : RightSpeed;
		LeftSpeed = MAXSPEED;
		Motor(RIGHTMOTOR, RightSpeed, FORWARD); // Turn Away from Wall
		Motor(LEFTMOTOR, LeftSpeed, FORWARD);
	}

	if (IRdist() > 2000) { //if something is detected in front of the robot
		Motor(RIGHTMOTOR, MAXSPEED, BACKWARD); // Turn clockwise away from wall
		Motor(LEFTMOTOR, MAXSPEED, FORWARD);
	}
}

void Timer3IntHandler(void){
	static int i=0;
	// Clear the timer interrupt and Disable
	TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	if(ctl == WALL){
//		if(i>=1){
			error= ultrasonic() - REF;
			rate = (ultrasonic() - previous)/TIME;
			previous = ultrasonic();
			integrator = integrator + error*TIME;
			FollowWall();
			i=0;
//		}
//		i++;
	}
	else if(ctl == LINE){
		FollowLeft();
	}
}

/////////////////////////////////////////////////////////////////
//Turns
//
void SharpTurnLeft(void){
	ShaftEncoder_Clear();
	// Turn Left 90 Degrees
	Motor(RIGHTMOTOR, 80, FORWARD);
	Motor(LEFTMOTOR, 80, BACKWARD);
	while(ShaftEncoder_Get()<5);
	// For .15 s
//	SysCtlDelay(SysCtlClockGet()/(6*3));
}
void SharpTurnRight(void){
	ShaftEncoder_Clear();
	// Turn Right 90 Degrees
	Motor(RIGHTMOTOR, 80, BACKWARD);
	Motor(LEFTMOTOR, 80, FORWARD);
	while(ShaftEncoder_Get()<7);
	// For 1 s
//	SysCtlDelay(SysCtlClockGet()/(4*3));
}

void SmoothTurnLeft(void){
	ShaftEncoder_Clear();
	// Turn Left 90 Degrees
	Motor(RIGHTMOTOR, 75, FORWARD);
	Motor(LEFTMOTOR, 40, FORWARD);
	while(ShaftEncoder_Get()<8);
	// For .5 s
//	SysCtlDelay(SysCtlClockGet()/(2*3));
}
void SmoothTurnRight(void){
	ShaftEncoder_Clear();
	// Turn Right 90 Degrees
	Motor(RIGHTMOTOR, 40, FORWARD);
	Motor(LEFTMOTOR, 75, FORWARD);
	while(ShaftEncoder_Get()<16);
	// For 2 s
//	SysCtlDelay(2*SysCtlClockGet()/(3*3));
}

void PanRight(void){
	ShaftEncoder_Clear();
	// Turn Right 45 Degrees
	Motor(RIGHTMOTOR, 65, BACKWARD);
	Motor(LEFTMOTOR, 65, FORWARD);
	while(ShaftEncoder_Get()<6);
}

void PanLeft(void){
	ShaftEncoder_Clear();
	// Turn Left 45 Degrees
	Motor(RIGHTMOTOR, 65, FORWARD);
	Motor(LEFTMOTOR, 65, BACKWARD);
	while(ShaftEncoder_Get()<3);
}

/////////////////////////////////////////////////////////////////
// Course Functions
//

void lineleftwall(void){
	// Follow Line Until the wall
	ctl=LINE;
	SysCtlDelay(SysCtlClockGet()/(100*3));
	ShaftEncoder_Clear();
	while(ShaftEncoder_Get()<17);

	ctl=NOTHING;
	//turn left

	SharpTurnLeft();
	// until wall
	Motor(BOTHMOTOR,100,FORWARD);
	while(ultrasonic() > 50000);
	//follow wall
	ctl=WALL;
	// delay for 100ms
	SysCtlDelay(SysCtlClockGet()/(5*3));
	while(ultrasonic() < NOWALL);
	UARTprintf("See Wall %u\r\n",ultrasonic());
	ctl=NOTHING;
	Motor(BOTHMOTOR, 100, BRAKE);
}

void linerightwall(void){
	// Follow Line Until the wall
	ctl=LINE;
	ShaftEncoder_Clear();
	while(ShaftEncoder_Get()<14);

	ctl=NOTHING;
	//turn Right

	SharpTurnRight();
	//Go Forward
	Motor(BOTHMOTOR,100,FORWARD);
	// until wall
	while(ultrasonic() > 50000);
	// follow wall
	ctl=WALL;
	// delay for 100ms
	SysCtlDelay(SysCtlClockGet()/(5*3));
	while(ultrasonic() < NOWALL);
	UARTprintf("See Wall %u\r\n",ultrasonic());
	ctl=NOTHING;
	Motor(BOTHMOTOR, 100, BRAKE);
}

void walllineleft(void){

	//Forward
	Motor(BOTHMOTOR, 80, FORWARD);
	SysCtlDelay(SysCtlClockGet()/600);
	//until either sensor see black line
	while((lineSensor(LEFT) == WHITE) || (lineSensor(RIGHT) == WHITE ));
	ShaftEncoder_Clear();
	while(ShaftEncoder_Get()<2);
	// Brake then Turn Left
	Motor(LEFTMOTOR,100,BRAKE);
	Motor(RIGHTMOTOR,75,FORWARD);
	SysCtlDelay(SysCtlClockGet()/600);
	//  till Left sensor is BLACK
	while (lineSensor(RIGHT) == BLACK);
	while (lineSensor(RIGHT) == WHITE);
	Motor(RIGHTMOTOR,100,BRAKE);
}

void walllineright(void){
	// Forward
	Motor(BOTHMOTOR, 80, FORWARD);
	SysCtlDelay(SysCtlClockGet()/600);
	//until either sensor see black line
	while((lineSensor(LEFT) == WHITE ) || (lineSensor(RIGHT) == WHITE ));
	ShaftEncoder_Clear();
	while(ShaftEncoder_Get()<2);
	// Turn Right
	Motor(RIGHTMOTOR,100,BRAKE);
	Motor(LEFTMOTOR,75,FORWARD);
	SysCtlDelay(SysCtlClockGet()/600);
	//  till Right sensor is BLACK
	while (lineSensor(LEFT) == BLACK);
	while (lineSensor(LEFT) == WHITE);
	Motor(LEFTMOTOR,100,BRAKE);
}
///////////////////////////////////////////////////////////////////
// Complete the Course
//

void CompleteCourse(void){
	int speed = 50;
	int wall = 75000;
	int room_count=1;
	Stop = 0;

	// Starting Towards Room 1
	// Follow left Edge of line to can
	// Detect Can and Leave Room
	//FollowToCan();
	can = RED;
	currentRoom = YELLOW;

	while( (Stop != 1) && (room_count <= 4)){
		room_count++;
		UARTprintf("Current Room = ");
		switch (currentRoom) {
		case RED:
			UARTprintf("RED    \t");
			break;
		case GREEN:
			UARTprintf("GREEN  \t");
			break;
		case BLUE:
			UARTprintf("BLUE   \t");
			break;
		case YELLOW:
			UARTprintf("YELLOW \t");
			break;
		}
		UARTprintf("Can Color = ");
		switch(can){
		case RED:
			UARTprintf("RED \r\n");
			if(currentRoom == 1){ // ERROR
				UARTprintf("Error: The  can is in the right room \r\n");
				StopCourse();
			}
			else if(currentRoom == 2){
				// Follow line, turn right to follow wall
				linerightwall();
				// turn right to line
				walllineright();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 1;
			}
			else if(currentRoom == 3){
				// Follow line, turn left to follow wall
				lineleftwall();
				// turn right to line
				walllineright();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 1;
			}
			else if(currentRoom == 4){
				// Follow Line, detect can and leave
				FollowToCan();
				currentRoom = 1;
			}
			break;
		case GREEN:
			UARTprintf("GREEN \r\n");
			if(currentRoom == 1){
				// Follow line, turn left to follow wall
				lineleftwall();
				// turn left to line
				walllineleft();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 2;
			}
			else if(currentRoom == 2){ // ERROR
				UARTprintf("Error: The  can is in the right room \r\n");
				StopCourse();
			}
			else if(currentRoom == 3){
				// Follow Line, detect can and leave
				FollowToCan();
				currentRoom = 2;
			}
			else if(currentRoom == 4){
				// Follow line, turn right to follow wall
				linerightwall();
				//turn right to line
				walllineleft();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 2;
			}
			break;
		case BLUE:
			UARTprintf("BLUE \r\n");
			if(currentRoom == 1){
				// Follow line, turn left to follow wall
				lineleftwall();
				//turn right to line
				walllineright();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 3;
			}
			else if(currentRoom == 2){
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 3;
			}
			else if(currentRoom == 3){ // ERROR
				UARTprintf("Error: The  can is in the right room \r\n");
				StopCourse();
			}
			else if(currentRoom == 4){
				// Follow line, turn right to follow wall
				linerightwall();
				//turn right to line
				walllineright();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 3;
			}
			break;
		case YELLOW:
			UARTprintf("YELLOW \r\n");
			if(currentRoom == 1){
				// Follow Line to Room
				FollowToCan();
				currentRoom = 4;
			}
			else if(currentRoom == 2){
				//Follow line, turn right to follow wall
				linerightwall();
				//turn left to line
				walllineleft();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 4;
			}
			else if(currentRoom == 3){
				// Follow line, turn left to follow wall
				lineleftwall();
				//turn left to line
				walllineleft();
				//Follow line to can, detect color, leave
				FollowToCan();
				currentRoom = 4;
			}
			else if(currentRoom == 4){// ERROR
				UARTprintf("Error: The  can is in the right room \r\n");
				StopCourse();
			}
			break;
		}
	}
	UARTprintf("Done \r\n");

}
///////////////////////////////////////////////////////////////////
// Course functions
//

void Hardcode(void) {
	char sequence, confirm_char;
	int i;
	fakeit = !fakeit;

	if ( fakeit ) {
		UARTprintf("Faking it\n\r");
		do {
			UARTprintf("Enter Color Sequence:");
			for (i=0;i<4;i++) {
				sequence = UARTgetc();
				UARTprintf("%c\n\r",sequence);
				switch ( sequence ) {
				case 'r':
					fake_color[i] = RED;
					break;
				case 'g':
					fake_color[i] = GREEN;
					break;
				case 'b':
					fake_color[i] = BLUE;
					break;
				case 'y':
					fake_color[i] = YELLOW;
					break;
				default:
					UARTprintf("Wrong Character '%c' ,do it again\n\r", sequence);
					i--;
					break;
				}
			}
			UARTprintf("Color Sequence: ");
			for (i=0;i<4;i++) {
				switch ( fake_color[i] ) {
				case RED:
					UARTprintf("RED ");
					break;
				case GREEN:
					UARTprintf("GREEN ");
					break;
				case BLUE:
					UARTprintf("BLUE ");
					break;
				case YELLOW:
					UARTprintf("YELLOW ");
					break;
				}
			}
			UARTprintf("(y,n)?");
			confirm_char = UARTgetc();
			UARTprintf("\n\r");
		} while (confirm_char != 'y');
	}
	else {
		UARTprintf("For Realsies now\n\r");
	}
}
void StopCourse(void){
	Motor(BOTHMOTOR, 0 ,BRAKE);
	Stop = 1;
}

