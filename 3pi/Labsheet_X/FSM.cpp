#include "FSM.h"
#include "motors.h"
#include "linesensor.h"
#include <Arduino.h>

FSM* FSM::instance = nullptr;

/*--------------------
       INTERFACE
----------------------*/
static bool FSM::gotoState()
{
  if(instance == nullptr)
    return false;
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
  case State::LINE_FINISHED: 
    return false;
  }
  #if DEBUG_FSM_C
  Serial.println(instance->state);
  #endif
  return true;
}

/*--------------------
     LINE MISSING
----------------------*/
#define BACKTRACK_THRESH 2000000 // 1 secs
void FSM::onLineMissing()
{
  #if DEBUG_FSM
  Serial.println("State: LINE_MISSING\n");
  //digitalWrite(LED_PIN_R, HIGH);
  #endif
  
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
  if(gsv[GSRR] < -0.5)
  {
    onLineJoin(true);
    return;
  }
  if(gsv[GSLL] < -0.5)
  {
    onLineJoin(false);
    return;
  }
  if(micros() - t_mark > BACKTRACK_THRESH)
    if(millis() < GAMEOVER_THRESHOLD) // A shoddy way of distinguishing between line end and line start
      onLineLostTurn();
    else
    {
      Motors::setRMotor(0);
      Motors::setLMotor(0);
      state = LINE_FINISHED;
    }
}

/*--------------------
       LINE LOST
----------------------*/
#define LLTURN_THRESH 1600000 // 1.6 secs
void FSM::onLineLostTurn()
{
  #if DEBUG_FSM
  Serial.println("State: LINE_LOST_TURN\n");
  //digitalWrite(LED_PIN_R, HIGH);
  #endif
  
  state = LINE_LOST_TURN;
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
  #if DEBUG_FSM
  Serial.println("State: LINE_LOST_TRAVEL\n");
  #endif
  
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
#define JOIN_TURN_TIME 431000 // 0.431 secs (50 deg turn)
void FSM::lineNone()
{
  if(gsv[GSRR] < -0.5)
  {
    onLineJoin(true);
    return;
  }
  else if(gsv[GSLL] < -0.5)
  {
    onLineJoin(false);
    return;
  }
}
void FSM::onLineJoin(bool go_right)
{
  #if DEBUG_FSM
  Serial.println("State: LINE_JOIN\n");
  //digitalWrite(LED_PIN_G, HIGH);
  //digitalWrite(LED_PIN_Y, HIGH);
  #endif
  
  state = LINE_JOIN;
  t_mark = micros();
  if(go_right)
  {
    Motors::setLMotor(SPEED);
    Motors::setRMotor(-SPEED);
  }
  else
  {
    Motors::setLMotor(SPEED);
    Motors::setRMotor(-SPEED);
  }
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
  #if DEBUG_FSM
  Serial.println("State: LINE_FOLLOW\n");
  //digitalWrite(LED_PIN_Y, HIGH);
  #endif
  state = LINE_FOLLOW;
}

void FSM::lineFollow()
{
  if(gsv[GSL] <= -1 &&
     gsv[GSC] <= -1 &&
     gsv[GSR] <= -1)
  {
    // all sensors inside the line
    // continue what we were doing last, until a sensor exits the line
    // ??or should be just turn until we see light??
    
    #if DEBUG_FSM
    Serial.println("Engulfed\n");
    #endif
    //Motors::setRMotor(-SPEED);
    //Motors::setLMotor(SPEED);
    //return;
  }
  
  if(gsv[GSL] >= -0.2 &&
     gsv[GSC] >= -0.2 &&
     gsv[GSR] >= -0.2) // what about just if(!checkForLine())
  {
    onLineMissing();
    return;
  }
  
  if(gsv[GSL] < 0 ||
     gsv[GSR] < 0)
  {
    lineTurn();
  }
  else if (gsv[GSC] < 0)
  {
    Motors::setRMotor(SPEED);
    Motors::setLMotor(SPEED);
  }  
}

void FSM::lineTurn()
{
#if LINETURN_ALPHA
  int LR_delta = gsv[GSL] - gsv[GSR];
  bool inverted = gsv[GSL] < gsv[GSR];
  if(inverted)
    LR_delta *= -1;
  float ratio = LR_delta;
  
  // use BinaryCrossEntropy-like blend
  
  Motors::setRMotor(SPEED * (inverted*(1-ratio) + (1-inverted)*ratio));
  Motors::setLMotor(SPEED * (inverted*ratio + (1-inverted)*(1-ratio)));
#else
  float e = (gsv[GSL] + 0.5f * gsv[GSC]) - (gsv[GSR] + 0.5f * gsv[GSC]);
  e /= 3;
  Motors::setRMotor(SPEED * (1+TURN_AGGRESSION*e));
  Motors::setLMotor(SPEED * (1+TURN_AGGRESSION*e));
#endif
}


/*--------------------
         UTILS
----------------------*/
// returns true if any sensors are below the threshold
bool FSM::checkForLine() 
{
  if(gsv[GSL] < -0.2 ||
     gsv[GSC] < -0.2 ||
     gsv[GSR] < -0.2)
  {
    return true;
  }
  return false;
}
