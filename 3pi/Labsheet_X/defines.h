#pragma once

// Pins
#define R_PWM_PIN A9
#define L_PWM_PIN A10
#define R_DIR_PIN 15
#define L_DIR_PIN 16
#define GROUND_L A0
#define GROUND_C A2
#define GROUND_R A3

#define LS_PIN_LL A11
#define LS_PIN_L A0
#define LS_PIN_C A2
#define LS_PIN_R A3
#define LS_PIN_RR A4
#define LS_PIN_EMIT

// Utils
#define FWD LOW
#define REV HIGH

// Magic Numbers
#define GS_COUNT 5
#define GS_WHITE 830  // TODO: make dynamic
#define GS_BLACK 305
#define GSLL 0
#define GSL  1
#define GSC  2
#define GSR  3
#define GSRR 4
