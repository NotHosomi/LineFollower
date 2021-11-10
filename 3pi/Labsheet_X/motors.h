#pragma once
#include "defines.h"
#include <Arduino.h>

// Class to operate the motor(s).
// Motor manager
class Motors {
  public:
  // Constructor, must exist.
  Motors() {}
  
  static void init() {
    pinMode(L_PWM_PIN, OUTPUT);
    pinMode(R_PWM_PIN, OUTPUT);
    pinMode(R_DIR_PIN, OUTPUT);
    pinMode(L_DIR_PIN, OUTPUT);
  
    digitalWrite(R_DIR_PIN, FWD);
    digitalWrite(L_DIR_PIN, FWD);
  
    analogWrite(L_PWM_PIN, 20);
    analogWrite(R_PWM_PIN, 20);
  }

  static void setLMotor(float pwm)
  {
    #if DEBUG_MOTORS
    Serial.print("L Motor ");
    Serial.println(pwm);
    #endif
    digitalWrite(L_DIR_PIN, pwm < 0); // REV == HIGH == 1 == true, FWD == LOW == 0 == false
    analogWrite(L_PWM_PIN, abs(pwm));
  }
  static void setRMotor(float pwm)
  {
    #if DEBUG_MOTORS
    Serial.print("R Motor ");
    Serial.println(pwm);
    #endif
    digitalWrite(R_DIR_PIN, pwm < 0); // REV == HIGH == 1 == true, FWD == LOW == 0 == false
    analogWrite(R_PWM_PIN, abs(pwm));
  }
};
