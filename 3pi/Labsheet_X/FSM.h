#pragma once


class FSM {
private:

public:
  FSM()
  {
    FSM::instance = this;
  }
  //~FSM();
  static bool gotoState();

  // TODO: get Serial working outside of .ino
  static FSM* instance;
  int gsv[5] = {0, 0, 0, 0, 0};
  
private:
  
  enum State
  {
    //PREP,
    LINE_NONE,       // proceed until line found
    LINE_JOIN,       // first hit line
    LINE_FOLLOW,     // we have a line
    LINE_MISSING,    // hoping for line
    LINE_LOST_TURN,  // returning to Line
    LINE_LOST_TRAVEL // returning to line
  };
  State state = LINE_NONE;

  // state transitions
  void onLineMissing();
  void onLineLostTurn();
  void onLineLostTravel();
  void onLineFollow();
  void onLineJoin();
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
