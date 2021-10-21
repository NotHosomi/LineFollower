#include FSM.h
#include motors.h

// returns true if any sensors are below the threshold
FSM::bool checkForLine()
{
  if(gsv[GSL] <= GS_WHITE ||
     gsv[GSC] <= GS_WHITE ||
     gsv[GSR] <= GS_WHITE)
  {
    return true;
  }
  return false;
}

/*--------------------
     LINE MISSING
----------------------*/
#define BACKTRACK_THRESH 2000000 // 2 secs
void FSM::onLineMissing()
{
  printf("State: LINE_MISSING\n");
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
  printf("State: LINE_LOST_TURN\n");
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
  printf("State: LINE_LOST_TRAVEL\n");
  state = LINE_LOST_TRAVEL;
  wb_motor_set_velocity(right_motor, SPEED);
  wb_motor_set_velocity(left_motor, SPEED);
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
  printf("State: LINE_JOIN\n");
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
  printf("State: LINE_FOLLOW\n");
  state = LINE_FOLLOW;
}

void FSM::lineFollow()
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
    wb_motor_set_velocity(right_motor, SPEED);
    wb_motor_set_velocity(left_motor, SPEED);
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
    wb_motor_set_velocity(right_motor, SPEED * (inverted*(1-ratio) + (1-inverted)*ratio));
    wb_motor_set_velocity(left_motor, SPEED * (inverted*ratio + (1-inverted)*(1-ratio)));
}



/*--------------------
     NON-SPECIFIC
----------------------*/
void FSM::gotoState()
{
  //printf("Tick @ %.2f\n", micros());
  // update sensor values
  for(int i = 0; i < GS_COUNT; ++i)
  {
    gsv[i] = wb_distance_sensor_get_value(gs[i]);
  }
  //printf(" L: %d  C: %d  R: %d\n", gsv[0], gsv[1], gsv[2]);
  switch(state)
  {
  case State::LINE_NONE: lineNone();
    break;
  case State::LINE_JOIN: lineJoin();
    break;
  case State::LINE_FOLLOW: lineFollow();
    break;
  case State::LINE_MISSING: lineMissing();
    break;
  case State::LINE_LOST_TURN: lineLostTurn();
    break;
  case State::LINE_LOST_TRAVEL: lineLostTravel();
    break;
  }
}
