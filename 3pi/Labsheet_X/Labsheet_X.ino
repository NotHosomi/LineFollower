#include "defines.h"
#include "FSM.h"
#include "motors.h"
#include "linesensor.h"
//#include "encoders.h"
//#include "kinematics.h"
//#include "pid.h"

#define LED_PIN 13
boolean led_state;

FSM fsm;




// put your setup code here, to run once:
void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set LED pin as an output
  pinMode( LED_PIN, OUTPUT );
  /*pinMode( L_PWM_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  
  digitalWrite(R_DIR_PIN, FWD);
  digitalWrite(L_DIR_PIN, FWD);
  
  analogWrite( L_PWM_PIN, 20 );
  analogWrite( R_PWM_PIN, 20 );*/
  Motors::init();
  LineSensors::init();
  new FSM();

  // Set initial state of the LED
  led_state = false;
}

int gsv[5] = { 0, 0, 0, 0, 0};

// put your main code here, to run repeatedly:
unsigned int dT = 0; // delta time
unsigned long pT = 0; // prev time
char buffer[100] = "";
void loop() {
  dT = micros() - pT;
  pT = micros();
  
  LineSensors::refresh(gsv);
  fsm.gotoState();

  
#if SERIAL_PLOT
  // TODO get serial working outside of .ino
  //sprintf(buffer, " LL: %d  L: %d  C: %d  R: %d  RR: %d", gsv[0], gsv[1], gsv[2], gsv[3], gsv[4]);
  sprintf(buffer, "%d %d %d %d %d\nLL L C R RR", gsv[0], gsv[1], gsv[2], gsv[3], gsv[4]);
  Serial.println(buffer);
#endif
  // Odomentry(dT);
  delay(50);
}
