/*
 * functions.h
 *
 *  Created on: Nov 2, 2013
 *      Author: Fairy_000
 */

#ifndef FUNCTIONS_H_

void Timer3IntHandler(void);

// Complete the Course
void CompleteCourse(void);
void Hardcode(void);

// Stop the Course
void StopCourse(void);

// Follow line to room, detect can,
// and leave room
void FollowToCan(void);

// Go into room to detect can
int DetectCan(void);

// Following the right edge with right sensor
void FollowLeft(void);
// Following the left edge with left sensor
void FollowRight(void);

void FollowWall(void);

void FollowINIT(void);

/////////////////////////////////////////////////////////////////
// course functions
//
void lineleftwall(void); // follow line, turn left, follow wall
void linerightwall(void); // follow line, turn right, follow wall
void walllineleft(void); // After following wall, find line to the left
void walllineright(void); // After following wall, find line to the right
/////////////////////////////////////////////////////////////////
//Turns about Axis
//
void SharpTurnLeft(void);
void SharpTurnRight(void);
//////////////////////////////////////////////////////////////////
// Smooth Turns while moving
void SmoothTurnLeft(void);
void SmoothTurnRight(void);
//////////////////////////////////////////////////////////////////
// Turn 45 Degrees
void PanLeft(void);
void PanRight(void);


#define WALL 0
#define LINE 1
#define NOTHING 2
#define FRONT 1
#define BACK 0


#endif /* FUNCTIONS_H_ */
