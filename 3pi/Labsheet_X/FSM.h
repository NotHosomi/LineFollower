#pragma once


class FSM {
private:

public:
  FSM() = default;
  void gotoState();
  
private:
  enum State
  {
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
};
