#include "defines.h"
//#include "FSM.h"
//#include "motors.h"
//#include "linesensor.h"
//#include "encoders.h"
//#include "kinematics.h"
//#include "pid.h"

#define LED_PIN 13
boolean led_state;






// put your setup code here, to run once:
void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set LED pin as an output
  pinMode( LED_PIN, OUTPUT );
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  
  digitalWrite(R_DIR_PIN, FWD);
  digitalWrite(L_DIR_PIN, FWD);
  
  analogWrite( L_PWM_PIN, 20 );
  analogWrite( R_PWM_PIN, 20 );

  // Set initial state of the LED
  led_state = false;
}


// put your main code here, to run repeatedly:
unsigned int dT = 0; // delta time
unsigned long pT = 0; // prev time
void loop() {
  dT = micros() - pT;
  pT = micros();
  analogWrite( L_PWM_PIN, 20 );
  analogWrite( R_PWM_PIN, 20 );

  Serial.println("Loop");
  delay(5);
}
