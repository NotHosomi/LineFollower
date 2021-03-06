//#include<stdlib.h>

/*
*
*   PLEASE NOTE
*   Obstacle avoidance is not implemented here,
*   as this was a prototype for the 3pi+ implementation
*
*   KNOWN ISSUES:
*   Struggles with 90* corners in the line
*   
*/

#include <stdio.h>
#include <webots/distance_sensor.h>
#include <webots/position_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define TIME_STEP 32  // [ms]


#define GS_COUNT 3
#define GS_WHITE 830
#define GS_BLACK 305
#define GSL 0
#define GSC 1
#define GSR 2
#define SPEED 1.0f
#define WHEEL_R 20.5 //mm
#define AXEL_L 52 //mm
WbDeviceTag gs[GS_COUNT]; /* ground sensors */
unsigned short gsv[GS_COUNT] = {0, 0, 0};
WbDeviceTag left_motor, right_motor;
WbDeviceTag left_pos, right_pos;
float t_mark; // timer

enum State
{
  LINE_NONE,       // proceed until line found
  LINE_JOIN,       // first hit line
  LINE_FOLLOW,     // we have a line
  LINE_MISSING,    // hoping for line
  LINE_LOST_TURN,  // returning to Line
  LINE_LOST_TRAVEL // returning to line
};
enum State state = LINE_NONE;

struct Kinematics {
  float pL;
  float pR;
  float dL;
  float dR;
  float x;
  float y;
  float th;
};
struct Kinematics pose;




int checkForLine()
{
  if(gsv[GSL] <= GS_WHITE ||
     gsv[GSC] <= GS_WHITE ||
     gsv[GSR] <= GS_WHITE)
  {
    return 1;
  }
  return 0;
}

/*--------------------
  DECLARATIONS
----------------------*/
// state transitions
void onLineMissing();
void onLineLostTurn();
void onLineLostTravel();
void onLineFollow();
void onLineJoin();
// states
void lineMissing();
void lineLostTurn();
void lineLostTravel();
void lineFollow();
void lineJoin();
void lineNone();

/*--------------------
     LINE MISSING
----------------------*/
#define BACKTRACK_THRESH 3 //secs
void onLineMissing()
{
  printf("State: LINE_MISSING\n");
  state = LINE_MISSING;
  t_mark = wb_robot_get_time();
  
  wb_motor_set_velocity(right_motor, SPEED);
  wb_motor_set_velocity(left_motor, SPEED);
}
void lineMissing()
{
  if(checkForLine())
  {
    onLineFollow();
    return;
  }
  if(wb_robot_get_time() - t_mark > BACKTRACK_THRESH)
    onLineLostTurn();
}

/*--------------------
       LINE LOST
----------------------*/
#define LLTURN_THRESH 0.5f
void onLineLostTurn()
{
  printf("State: LINE_LOST_TURN\n");
  t_mark = wb_robot_get_time();
  wb_motor_set_velocity(right_motor, SPEED);
  wb_motor_set_velocity(left_motor, -SPEED);
}
void lineLostTurn()
{
  // No change in actions until timer elapses
  if(wb_robot_get_time() - t_mark > LLTURN_THRESH)
    onLineLostTravel();
}
void onLineLostTravel()
{
  printf("State: LINE_LOST_TRAVEL\n");
  state = LINE_LOST_TRAVEL;
  wb_motor_set_velocity(right_motor, SPEED);
  wb_motor_set_velocity(left_motor, SPEED);
}
void lineLostTravel()
{
  if(checkForLine())
  {
    onLineFollow();
    return;
  }
}


/*--------------------
     LINE JOINING
----------------------*/
#define JOIN_TURN_TIME 3.0f
void lineNone()
{
  if(checkForLine())
  {
    onLineJoin();
    return;
  }
}
void onLineJoin()
{
  printf("State: LINE_JOIN\n");
  state = LINE_JOIN;
  t_mark = wb_robot_get_time();
}
void lineJoin()
{
  if(wb_robot_get_time() - t_mark > JOIN_TURN_TIME)
  {
    onLineFollow();
    return;
  }
}

/*--------------------
      LINE FOLLOW
----------------------*/

void lineTurn();

void onLineFollow()
{
  printf("State: LINE_FOLLOW\n");
  state = LINE_FOLLOW;
}

void lineFollow()
{
  if(gsv[GSL] <= GS_BLACK &&
     gsv[GSC] <= GS_BLACK &&
     gsv[GSR] <= GS_BLACK)
  {
    // all sensors inside the line
    // continue what we were doing last, until a sensor exits the line
    // ??or should be just turn until we see light??
    printf("Engulfed\n");
    wb_motor_set_velocity(right_motor, -SPEED);
    wb_motor_set_velocity(left_motor, SPEED);
    return;
  }
  
  if(gsv[GSL] >= GS_WHITE &&
     gsv[GSC] >= GS_WHITE &&
     gsv[GSR] >= GS_WHITE)
  {
    onLineMissing();
    return;
  }
  
  if(gsv[GSL] <= GS_WHITE ||
     gsv[GSR] <= GS_WHITE)
  {
    lineTurn();
  }
  else if (gsv[GSC] <= GS_WHITE)
  {
    wb_motor_set_velocity(right_motor, SPEED);
    wb_motor_set_velocity(left_motor, SPEED);
  }  
}

void lineTurn()
{
    int LR_delta = gsv[GSL] - gsv[GSR];
    int inverted = gsv[GSL] < gsv[GSR];
    if(inverted)
      LR_delta *= -1;
    float ratio = (GS_WHITE - LR_delta) / GS_WHITE;
    
    // use BinaryCrossEntropy-like blend     
    wb_motor_set_velocity(right_motor, SPEED * (inverted*(1-ratio) + (1-inverted)*ratio));
    wb_motor_set_velocity(left_motor, SPEED * (inverted*ratio + (1-inverted)*(1-ratio)));
}



/*--------------------
     NON-SPECIFIC
----------------------*/
void gotoState()
{
  switch(state)
  {
  case LINE_NONE: lineNone();
    break;
  case LINE_JOIN: lineJoin();
    break;
  case LINE_FOLLOW: lineFollow();
    break;
  case LINE_MISSING: lineMissing();
    break;
  case LINE_LOST_TURN: lineLostTurn();
    break;
  case LINE_LOST_TRAVEL: lineLostTravel();
    break;
  }
}

void updatePose()
{
  float L = wb_position_sensor_get_value(left_pos);
  float R = wb_position_sensor_get_value(right_pos);
  pose.dL = L - pose.dL;
  pose.dR = R - pose.dR;
  pose.pL = L;
  pose.pR = R;
  
  // wheel radius = 20.5
  // axle length = 52
  float dist = (WHEEL_R * L / 2) + (WHEEL_R * R / 2);
  pose.x += dist * cos(pose.th);
  pose.y += dist * sin(pose.th);
  
  pose.th += (WHEEL_R * L / AXEL_L) - (WHEEL_R * R / AXEL_L);
  
  printf("X: %f  Z: %f th:%f\n", pose.x + -0.191267 * 1000, pose.y + 0.276446 * 1000, pose.th*180/3.14159265359);
}


void update()
{
  updatePose();
  //printf("Tick @ %.2f\n", wb_robot_get_time());
  // update sensor values
  for(int i = 0; i < GS_COUNT; ++i)
  {
    gsv[i] = wb_distance_sensor_get_value(gs[i]);
  }
  //printf(" L: %d  C: %d  R: %d\n", gsv[0], gsv[1], gsv[2]);
  gotoState();
}

// UTIL
void delay_ms( float ms ) {
  float millis_now;
  float millis_future;
  
  millis_now = wb_robot_get_time() * 1000.0;
  millis_future = millis_now + ms;
  
  // Wait for the elapsed time to occur
  // Note, steps simulation, so blocks
  // any further updates the rest of the code.
  while( millis_now < millis_future ) {
    millis_now = wb_robot_get_time() * 1000.0;
    wb_robot_step( TIME_STEP );
  } 
  
  return;
}

int main(void)
{
  printf("Begin\n");
  wb_robot_init();
  char id[3] = "";
  for (int i = 0; i < GS_COUNT; i++) {
    sprintf(id, "gs%d", i);
    gs[i] = wb_robot_get_device(id); /* ground sensors */
    wb_distance_sensor_enable(gs[i], TIME_STEP);
  }
  // motors
  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, SPEED);
  wb_motor_set_velocity(right_motor, SPEED);
  
  // wheels
  left_pos = wb_robot_get_device("left wheel sensor");
  right_pos = wb_robot_get_device("right wheel sensor");
  wb_position_sensor_enable(left_pos, TIME_STEP);
  wb_position_sensor_enable(right_pos, TIME_STEP);
  
  pose.pL = wb_position_sensor_get_value(left_pos);
  pose.pR = wb_position_sensor_get_value(right_pos);
  pose.dL = 0;
  pose.dR = 0;
  
  pose.x = 0;  // kinematic frame based on start pos
  pose.y = 0;
  pose.th = 0;
  
  while (wb_robot_step(TIME_STEP) != -1)
  {
    update();
  }
  wb_robot_cleanup();
  return 0;
}