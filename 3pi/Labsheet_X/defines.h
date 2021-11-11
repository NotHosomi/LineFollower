#pragma once
#include <Arduino.h>

#define DEBUG_GROUND false
#define DEBUG_FSM    false
#define DEBUG_FSM_C  false
#define DEBUG_MOTORS false
#define DEBUG_ENCODE false
#define DEBUG_ODO    true

#define LINETURN_ALPHA true
#define TURN_AGGRESSION 0.5
#define GAMEOVER_THRESHOLD 45000 // 45 seconds
#define SPEED 20

// Pins
#define LED_PIN_Y 13
#define LED_PIN_G 30
#define LED_PIN_R 17
#define PIN_BUZZ 6
#define R_PWM_PIN 9 // A9
#define L_PWM_PIN 10 // A10
#define R_DIR_PIN 15
#define L_DIR_PIN 16

#define LS_COUNT 5
#define LS_PIN_LL 12 //A11 //29
#define LS_PIN_L 18 // A0
#define LS_PIN_C 20 // A2
#define LS_PIN_R 21 // A3
#define LS_PIN_RR 22 // A4
#define LS_PIN_EMIT 11

// Utils
#define FWD LOW
#define REV HIGH

// Magic Numbers
#define GS_COUNT 5
#define GS_WHITE -780  // TODO: make dynamic
#define GS_BLACK -1000
#define GSLL 0
#define GSL  1
#define GSC  2
#define GSR  3
#define GSRR 4
#define SCALE_LL 750.0
#define SCALE_L  1000.0
#define SCALE_C  1100.0
#define SCALE_R  1200.0
#define SCALE_RR 1100.0

#define DARK false
#if DARK
  #define MIN_LL -800.0
  #define MIN_L  -600.0
  #define MIN_C  -600.0
  #define MIN_R  -800.0
  #define MIN_RR -1000.0
#else
  #define MIN_LL -500.0
  #define MIN_L  -500.0
  #define MIN_C  -500.0
  #define MIN_R  -600.0
  #define MIN_RR -600.0
#endif
