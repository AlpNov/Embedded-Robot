#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "usr_uart.h"
#include "modbus.h"
#include "cmd_interp.h"
#include "line.h"
#include "motor.h"
#include "ultrasonic.h"
#include "color.h"
#include "IRdist.h"
#include "shaft_encoder.h"
#include "functions.h"

extern int ctl;

struct cmd_to_fn
{
	char cmd[3];
	void (*fn)(void);
	char explain[40];

};

static const struct cmd_to_fn lookup[]=
{
	{"HP", &Print_Help, "Print this Help Message"},
	//Goofing off
	{"YO", &Hello, "Hello World"},

	//Motor Functions
	{"FW", &Forward, "Motors Forward"},
	{"BW", &Backward, "Motors Backward"},
	{"LF", &LeftForward, "Motors Left Forward"},
	{"RB", &RightBackward, "Motors Right Backward"},
	{"BR", &Brake, "Motors Brake"},
	{"CS", &Coast, "Motors Coast"},
	{"SL", &Slower, "Motors Slow down"},
	{"FS", &Faster, "Motors Speed Up"},

	//Line Sensor Functions
	{"LL", &Line_left, "Left Line Sensor"},
	{"LR", &Line_right, "Right Line Sensor"},

	//Shaft Encoder Function
	{"PE", &PrintEncoder, "Print Encoder 5 times"},
	{"TL", &TurnLeft, "Turn left for encoder count"},
	{"TR", &TurnRight, "Turn right for encoder count"},

	//Untrasonic Test
	{"UT", &PrintUltrasonic, "Ultrasonic Print 2"},

	//IR Distance Test
	{"IR", &IRTest, "IR Distnace print 25"},

	//Color Sensor Test
	{"CT", &ColorTest, "Color Sensor Print 5"},
	{"GC", &PrintColor, "Find Color"},

	//Line Following Test
	{"LT", &FollowTest, "Following Line"},

	//Wall Following Test Functions
	{"WF", &WallTest, "Following Wall"},
	{"ST", &StopTest, "Stop Test"},

	//Combined Test functions
	{"FC", &FollowToCan, "Follow Line to Can"},
	{"CD", &DetectCanTest, "Detect Can"},


	{"CF", &ToCanTest, "Follow Left Edge of line to Can"},
	{"LS", &SharpTurnTest, "Sharp Turns"},
	{"LW", &SmoothTurnTest, "Smooth Turns"},
	{"PT", &PanTest, "Pan 45 Degrees"},
	{"GW", &TimedForward, "Go forward for 1 Second"},
	{"GO", &CompleteCourse, "Complete the Course"},
	{"ST", &StopCourse, "Stop the Course"},
	{"WW", &lineleftwall, "line left to wall"},
	{"RW", &linerightwall, "line right to wall"},
	{"WL", &walllineleft, "Veer left to line"},
	{"WR", &walllineright, "Veer right to line"},
	{"HC", &Hardcode, "Enter Color Sequence"},

	//Done, invalid functions
	{"00", 0, "Invalid Tag"}
};
//*****************************************************************************
// Interpret - take in string, execute function
// YO - Hello
//
//*****************************************************************************

void Interpret (char* tag) {
	int i=0;
	while ( (lookup[i].cmd[0] != '0') && (lookup[i].cmd[1] != '0' ) ) {
		//	for(i=0; i< SIZE; i++){
		if((lookup[i].cmd[0] == tag[0]) && (lookup[i].cmd[1] ==tag[1])){
			(*lookup[i].fn)();
			return;
		}
		i++;
	}
	Invalid(tag);
} //end Interpret

//****************************************************************************
// Detect Can Test Functions
void DetectCanTest(void){
	DetectCan();
	Motor(BOTHMOTOR, 0, BRAKE);
}
void ToCanTest(void){
	// Go into room, detect can and leave room 
	FollowToCan();
	// STOP
	Motor(BOTHMOTOR, 60, BRAKE);
} // End ToCanTest

//****************************************************************************
// Wall Follow function
void WallTest(void){
	extern int ctl;
	ctl= WALL;
}

void StopTest(void){
	extern int ctl;
	ctl= NOTHING;
	Motor(BOTHMOTOR,0,BRAKE);
}
//****************************************************************************
// Follow Line Test function
void FollowTest(void){
	int lineleft,lineright;
	ctl = LINE;
	//ShaftEncoder_Clear();
	do {
	lineleft = lineSensor(LEFT);
	lineright = lineSensor(RIGHT);
	} while(lineleft == WHITE || lineright == WHITE);
	ctl = NOTHING;
	Motor(BOTHMOTOR,100,BRAKE);
}
//****************************************************************************
//  Turn Test Functions
void SharpTurnTest(){
	SharpTurnLeft();
	Motor(BOTHMOTOR,100,BRAKE);
	SharpTurnRight();
	Motor(BOTHMOTOR,100,BRAKE);
}
void SmoothTurnTest(){
	SmoothTurnLeft();
	Motor(BOTHMOTOR,100,BRAKE);
	// Pause 1 Second
	SysCtlDelay(SysCtlClockGet()/(3));
	SmoothTurnRight();
	Motor(BOTHMOTOR,100,BRAKE);
}

void PanTest(){
	PanLeft();
	Motor(BOTHMOTOR,0,BRAKE);
	PanRight();
	Motor(BOTHMOTOR,0,BRAKE);
}

//****************************************************************************
// Motor Control functions
static unsigned int speed=50;
void Forward(){
	Motor(BOTHMOTOR,speed,FORWARD);
}
void Backward(){
	Motor(BOTHMOTOR,speed,BACKWARD);
}
void LeftForward(){
	Motor(LEFTMOTOR,speed,FORWARD);
}
void RightBackward(){
	Motor(RIGHTMOTOR,speed,BACKWARD);
}
void Brake(){
	Motor(BOTHMOTOR,speed,BRAKE);
}
void Coast(){
	Motor(BOTHMOTOR,0,BRAKE);
}
void TimedForward(void){
	ShaftEncoder_Clear();
	Motor(BOTHMOTOR,75,FORWARD);
	while(ShaftEncoder_Get()<18);
	Motor(BOTHMOTOR,0, BRAKE);

}
void Slower() {
	if (speed > 10)
		speed -= 10;
	else
		speed = 0;
	Motor(LEFTMOTOR,GetMotorDirection(LEFTMOTOR) != BRAKE ? speed : 0,GetMotorDirection(LEFTMOTOR));
	Motor(RIGHTMOTOR,GetMotorDirection(RIGHTMOTOR) != BRAKE ? speed : 0,GetMotorDirection(RIGHTMOTOR));
}
void Faster() {
	if (speed < 90)
		speed += 10;
	else
		speed = 100;
	Motor(LEFTMOTOR,GetMotorDirection(LEFTMOTOR) != BRAKE ? speed : 0,GetMotorDirection(LEFTMOTOR));
	Motor(RIGHTMOTOR,GetMotorDirection(RIGHTMOTOR) != BRAKE ? speed : 0,GetMotorDirection(RIGHTMOTOR));
}
//*****************************************************************************

//****************************************************************************
// Shaft Encoder
void PrintEncoder() {
	int i;
	ShaftEncoder_Clear();
	for (i=0;i<5;i++) {
		UARTprintf("Shaft Encoder %u\r\n", ShaftEncoder_Get());
		SysCtlDelay(SysCtlClockGet()/15);
	}
}

#define TURN 12
void TurnLeft() {
	ShaftEncoder_Clear();
	Motor(RIGHTMOTOR,100,FORWARD);
	Motor(LEFTMOTOR,100,BACKWARD);
	while(ShaftEncoder_Get()<TURN);
	Motor(BOTHMOTOR,100,BRAKE);
	Motor(BOTHMOTOR,0,COAST);
}
void TurnRight() {
	ShaftEncoder_Clear();
	Motor(RIGHTMOTOR,100,BACKWARD);
	Motor(LEFTMOTOR,100,FORWARD);
	UARTprintf("Shaft Encoder %u\r\n", ShaftEncoder_Get());
	while(ShaftEncoder_Get()<TURN) {
	UARTprintf("Shaft Encoder %u\r\n", ShaftEncoder_Get());
	}
	Motor(BOTHMOTOR,100,BRAKE);
	Motor(BOTHMOTOR,0,COAST);
}
//****************************************************************************


//****************************************************************************
// IR Sensor Test Function
void IRTest(void){
	unsigned long value;
	int i;

	for(i=0; i < 5; i++){
		value = IRdist();
		UARTprintf("IR Value Raw: %u\n\r", value);
		// Wait 250 ms
		SysCtlDelay(SysCtlClockGet()/12);
	}
}

//****************************************************************************

//****************************************************************************
// Color Sensor Test Function
void ColorTest(void){
	int i, r, g, b, RG, RB, GB;

	UARTprintf("  R  |  G  |  B  | R/G | R/B | G/B  \n\r");
	UARTprintf("_____|_____|_____|_____|_____|______\n\r");
	for(i=0; i<5; i++){
		// get colors
		r=Red();
		b=Blue();
		g=Green();

		RG= r*100/g;
		RB= r*100/b;
		GB= g*100/b;

		UARTprintf("%5u|%5u|%5u|%5u|%5u|%5u\t",r,g,b, RG, RB, GB);
		UARTprintf("\n\r");
	}
}

void PrintColor(void) {
	unsigned int color;
	color = GetColor();
	switch ( color ) {
	case RED:
		UARTprintf("RED\r\n");
		break;
	case GREEN:
		UARTprintf("GREEN\n\r");
		break;
	case BLUE:
		UARTprintf("BLUE\r\n");
		break;
	case YELLOW:
		UARTprintf("YELLOW\r\n");
		break;
	default:
		UARTprintf("UNKNOWN\r\n");
	}

}
//****************************************************************************

//****************************************************************************
// Line Sensor Test Functions
void Line_left(void){
	if ( lineSensor(LEFT) )
		UARTprintf("Line Left BLACK\n\r");
	else
		UARTprintf("Line Left WHITE\n\r");
}


 void Line_right(void){
	 if ( lineSensor(RIGHT) )
	 		UARTprintf("Line Right BLACK\n\r");
	 	else
	 		UARTprintf("Line Right WHITE\n\r");
}
//****************************************************************************

 //****************************************************************************
 // Ultrasonic Test Function
 void PrintUltrasonic() {
	 unsigned long distance,distance_cm;
	 int i;

	 for(i=0; i<2; i++){
		 distance = ultrasonic(); //distance in term of clock cycles
		 UARTprintf("Ultrasonic Raw: %u\t",distance);

		 distance_cm = (distance/(SysCtlClockGet()/1000000))/58;
		 UARTprintf("cm: %u\n\r",distance_cm);

		 // Wait 250 ms
		 SysCtlDelay(SysCtlClockGet()/12);
	 }
 }
 //****************************************************************************


//*****************************************************************************
// Hello - print "Hello World!!!"
//*****************************************************************************
void Hello(){
	MB_Put((unsigned char *)"Hello Mr. Programmer!!!");
}

//*****************************************************************************
// Print_Help - Print command menu
//*****************************************************************************
void Print_Help() {
	int i = 0;
	while ( (lookup[i].cmd[0] != '0') && (lookup[i].cmd[1] != '0' ) ) {
		UARTprintf("%s\t- %s\r\n",lookup[i].cmd,lookup[i].explain);
		i++;
	}
}


//*****************************************************************************
// Invalid - Invalid function tag
//*****************************************************************************
void Invalid(char * tag) {
	MB_Put((unsigned char *)"Invalid Function Tag!");
	MB_Put((unsigned char *) tag);
}

