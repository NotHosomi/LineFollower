#include "defines.h"
#include "FSM.h"
#include "motors.h"
#include "linesensor.h"
#include "kinematics.h"
//#include "pid.h"
#include "Grid.h"

boolean led_state;
Kinematics_c odometry;
Grid grid;


float t = 0;

// put your setup code here, to run once:
void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set LED pin as an output
  pinMode( LED_PIN_Y, OUTPUT );
  pinMode( LED_PIN_G, OUTPUT );
  pinMode( LED_PIN_R, OUTPUT );
  pinMode( PIN_BUZZ, OUTPUT );
  digitalWrite(LED_PIN_Y, LOW);
  digitalWrite(LED_PIN_G, LOW);
  digitalWrite(LED_PIN_R, LOW);
  digitalWrite(PIN_BUZZ, LOW);
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
  odometry = Kinematics_c();
  new FSM();

  // Set initial state of the LED
  led_state = false;

  
  Motors::setRMotor(0);
  Motors::setLMotor(0);
  delay(5000);
  //Motors::setLMotor(SPEED);
  //Motors::setRMotor(SPEED);
  t = micros();
}

double gsv[5] = { 0, 0, 0, 0, 0};

// put your main code here, to run repeatedly:
unsigned int dT = 0; // delta time
unsigned long pT = 0; // prev time
char buffer[100] = "";
void loop() {
  //dT = micros() - pT;
  //pT = micros();
  LineSensors::refresh(gsv);
  //FSM::gotoState();
  odometry.update();

  // odometry test
  //if(micros() - t > 3000000)
  //{
  //  Motors::setRMotor(0);
  //  Motors::setLMotor(0);
  //}
  
  delay(32);
}
