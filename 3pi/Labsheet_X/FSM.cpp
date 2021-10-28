#include "FSM.h"
#include "motors.h"
#include "linesensor.h"
#include <Arduino.h>

/*
FSM::FSM()
{
  if(instance)
    delete this;
  else
    instance = this;
}

FSM::~FSM()
{
  instance = nullptr;
}*/


FSM* FSM::instance = nullptr;

/*--------------------
       INTERFACE
----------------------*/
static bool FSM::gotoState()
{
  if(instance == nullptr)
    return true;
  //printf("Tick @ %.2f\n", micros());
  // update sensor values
  LineSensors::refresh(FSM::instance->gsv);
  switch(instance->state)
  {
  case State::LINE_NONE: instance->lineNone();
    break;
  case State::LINE_JOIN: instance->lineJoin();
    break;
  case State::LINE_FOLLOW: instance->lineFollow();
    break;
  case State::LINE_MISSING: instance->lineMissing();
    break;
  case State::LINE_LOST_TURN: instance->lineLostTurn();
    break;
  case State::LINE_LOST_TRAVEL: instance->lineLostTravel();
    break;
  }
  return false;
}

/*--------------------
     LINE MISSING
----------------------*/
#define BACKTRACK_THRESH 2000000 // 2 secs
void FSM::onLineMissing()
{
  //Serial.println("State: LINE_MISSING\n");
  state = LINE_MISSING;
  t_mark = micros();

  Motors::setRMotor(SPEED);
  Motors::setLMotor(SPEED);
}
void FSM::lineMissing()
{
  if(checkForLine())
  {
    onLineFollow();
    return;
  }
  if(micros() - t_mark > BACKTRACK_THRESH)
    onLineLostTurn();
}

/*--------------------
       LINE LOST
----------------------*/
#define LLTURN_THRESH 500000 // 0.5 secs
void FSM::onLineLostTurn()
{
  //Serial.println("State: LINE_LOST_TURN\n");
  t_mark = micros();
  Motors::setRMotor(SPEED);
  Motors::setLMotor(-SPEED);
}
void FSM::lineLostTurn()
{
  // No change in actions until timer elapses
  if(micros() - t_mark > LLTURN_THRESH)
    onLineLostTravel();
}
void FSM::onLineLostTravel()
{
  //Serial.println("State: LINE_LOST_TRAVEL\n");
  state = LINE_LOST_TRAVEL;
  Motors::setRMotor(SPEED);
  Motors::setLMotor(SPEED);
}
void FSM::lineLostTravel()
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
#define JOIN_TURN_TIME 3000000 // 3 secs
void FSM::lineNone()
{
  if(checkForLine())
  {
    onLineJoin();
    return;
  }
}
void FSM::onLineJoin()
{
  //Serial.println("State: LINE_JOIN\n");
  state = LINE_JOIN;
  t_mark = micros();
}
void FSM::lineJoin()
{
  if(micros() - t_mark > JOIN_TURN_TIME)
  {
    onLineFollow();
    return;
  }
}

/*--------------------
      LINE FOLLOW
----------------------*/
void FSM::onLineFollow()
{
  //Serial.println("State: LINE_FOLLOW\n");
  state = LINE_FOLLOW;
}

void FSM::lineFollow()
{
  float err = calcErr();
  if(gsv[GSL] <= GS_BLACK &&
     gsv[GSC] <= GS_BLACK &&
     gsv[GSR] <= GS_BLACK)
  {
    // all sensors inside the line
    // continue what we were doing last, until a sensor exits the line
    // ??or should be just turn until we see light??
    //printf("Engulfed\n");
  Motors::setRMotor(-SPEED);
  Motors::setLMotor(SPEED);
    return;
  }
  
  if(gsv[GSL] >= GS_WHITE &&
     gsv[GSC] >= GS_WHITE &&
     gsv[GSR] >= GS_WHITE) // what about just if(!checkForLine())
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
    Motors::setRMotor(SPEED);
    Motors::setLMotor(SPEED);
  }  
}

void FSM::lineTurn()
{
    int LR_delta = gsv[GSL] - gsv[GSR];
    int inverted = gsv[GSL] < gsv[GSR];
    if(inverted)
      LR_delta *= -1;
    float ratio = (GS_WHITE - LR_delta) / GS_WHITE;
    
    // use BinaryCrossEntropy-like blend
    
    Motors::setRMotor(SPEED * (inverted*(1-ratio) + (1-inverted)*ratio));
    Motors::setLMotor(SPEED * (inverted*ratio + (1-inverted)*(1-ratio)));
}


/*--------------------
         UTILS
----------------------*/
// returns true if any sensors are below the threshold
bool FSM::checkForLine() 
{
  if(gsv[GSL] <= GS_WHITE ||
     gsv[GSC] <= GS_WHITE ||
     gsv[GSR] <= GS_WHITE)
  {
    return true;
  }
  return false;
}

float FSM::calcErr()
{
  return (gsv[GSL] + (gsv[GSC] * 0.5f)) - (gsv[GSR] + (gsv[GSC]*0.5f)); 
}
