#pragma once

#define SERIAL_PLOT true

// Pins
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
#define SPEED 20
#define GS_COUNT 5
#define GS_WHITE -780  // TODO: make dynamic
#define GS_BLACK -1000
#define GSLL 0
#define GSL  1
#define GSC  2
#define GSR  3
#define GSRR 4
