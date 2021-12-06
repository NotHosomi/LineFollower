#pragma once
#include "defines.h"
#include <Arduino.h>


class FSM {
private:

public:
  FSM(double* p_gsv)
  {
    FSM::instance = this;
    gsv = p_gsv; // local accessor to the global ground sensor value array
  }
  //~FSM();
  static bool gotoState();

  // TODO: get Serial working outside of .ino
  static FSM* instance;
  double* gsv;//[5] = {0, 0, 0, 0, 0};
  
private:
  
  enum State
  {
    //PREP,
    LINE_NONE,       // proceed until line found
    LINE_JOIN,       // first hit line
    LINE_FOLLOW,     // we have a line
    LINE_MISSING,    // hoping for line
    LINE_LOST_TURN,  // returning to Line
    LINE_LOST_TRAVEL,// returning to line
    LINE_FINISHED
  };
  State state = LINE_NONE;

  // state transitions
  void onLineMissing();
  void onLineLostTurn();
  void onLineLostTravel();
  void onLineFollow();
  void onLineJoin(bool go_right);
  // state processing
  void lineMissing();
  void lineLostTurn();
  void lineLostTravel();
  void lineFollow();
  void lineJoin();
  void lineNone();

  // utils
  void lineTurn();
  bool checkForLine();
  float calcErr();
  
  unsigned long t_mark; // timer

};
