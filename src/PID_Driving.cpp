#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include <HardwareSerial.h>

// #include "RotaryEncoder.h"
// #include "Motor.h"
// #include "robotConstants.h"

#include <robotConstants.h>
#include <Motor.h>
#include <RotaryEncoder.h>


//test code to try smth
String board;
int boardNum = -1;

#ifdef ESP32

board = "esp32";
boardNum = 1;

#endif

#ifndef ESP32

// board = "BP";
// boardNum = 2;

#define G 3

#endif

#ifdef ARDUINO_ARCH_STM32

#define H 1

#endif
// end test

/*  BP pin defs for UART */

#define RX PB11
#define TX PB10

HardwareSerial SerialPort(USART3);
String msg;

/* Variables for station detection */
int currentStation = 0;
int targetStation = 0;
bool stopped = false;
bool direction = false; // false is forward, true is backward

/*  Function Declerations  */
void updateEncoder();
void ISRUpdateEncoder();
void ISRButton();
void tapeDetectedLB();
void tapeDetectedLA();


//NEED TO FIX THIS VARIABLE
bool buttonPressed = false;
//NEED TO FIX THIS VARIABLE


/*  PID Control Values  */


//use lecture slide to tune

double forwardError = 0.0;
double forwardLastError = 0.0;

double max_I = 140;

double forward_p, forward_d, forward_i;

double forward_g;

//

double backwardError = 0.0;
double backwardLastError = 0.0;

double backward_p, backward_d, backward_i;

double backward_g;


/*  Object declerations  */

encoder::RotaryEncoder encoder1(PB_8, PB_9);
movement::Motor MotorL(MotorL_P1, MotorL_P2);//, &encoder1);
movement::Motor MotorR(MotorR_P1, MotorR_P2);


void setup() {

  SerialPort.begin(115200);



	// Setup Serial Monitor
	Serial.begin(115200);
  Serial.println("Hello" + String(BOARD_NAME));

  Serial.println(boardNum);

  /*  Pot Pin  */
  pinMode(POT_PIN, INPUT);


  /*  Motor Pins  */
  pinMode(MotorL.getPinA(), OUTPUT);
  pinMode(MotorL.getPinB(), OUTPUT);

  pinMode(MotorR.getPinA(), OUTPUT);
  pinMode(MotorR.getPinB(), OUTPUT);



  MotorL.off();
  MotorR.off();
  delay(500);
  MotorL.forward(3000);
  delay(100);
  MotorL.off();
  MotorR.forward(3000);
  delay(100);
  MotorR.off();

  /*  Encoders  */
	pinMode(ROTARY_A, INPUT);
	pinMode(ROTARY_B, INPUT);
  // pinMode(encoder1.getPinA(), INPUT);
	// pinMode(encoder1.getPinB(), INPUT);

  pinMode(BUTTON_PIN, INPUT);

  targetStation = 1; // test driving to first tape
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISRButton, RISING);

	attachInterrupt(digitalPinToInterrupt(ROTARY_A), ISRUpdateEncoder, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ROTARY_B), ISRUpdateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TAPE_LB), tapeDetectedLB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TAPE_LA), tapeDetectedLA, CHANGE);

}


void loop() {

const int forwardMidMotorSpeed = 3300;
const int backwardMidMotorSpeed = 3300;

/*  driving forwards  */
if (false) {

  forwardError = (double) analogRead(FRONT_TAPE_SENSOR_1) - analogRead(FRONT_TAPE_SENSOR_2);


  double FORWARD_LOOP_GAIN = 1.0;
  double FORWARD_P_GAIN = 0.55;//1.4 goes very slowl
  double FORWARD_I_GAIN = 0.0;
  double FORWARD_D_GAIN = 1.36;//0.9;//0.4;//0.7;//0.9;//1.8;//1.9;//2.5;//2.0;//1.8;//0.9;//0.7

  forward_p = FORWARD_P_GAIN * forwardError;
  forward_d = FORWARD_D_GAIN * (forwardError - forwardLastError);
  forward_i = FORWARD_I_GAIN * forwardError + forward_i; //const * error + previous int value
  if (forward_i > max_I) {forward_i = max_I;}
  if (forward_i < -max_I) {forward_i = -max_I;}

  forward_g = FORWARD_LOOP_GAIN * ( forward_p + forward_i + forward_d ); 
  forwardLastError = forwardError; 

  //SEND MOTOR VALS

  MotorL.forward( (forwardMidMotorSpeed - 1 * forward_g) );
  MotorR.forward(  1 / 1.3 * ( ( forwardMidMotorSpeed + 1 * forward_g) ) );


  /*  SerialPort & Serial Monitor prints  */
  {

  int mL = forwardMidMotorSpeed - forward_g;
  int mR = forwardMidMotorSpeed + forward_g;
  // SerialPort.println("g: " + String( g) );
  // SerialPort.println("m1: " + String( midMotorSpeed - g) );
  // SerialPort.println("m2: " + String( midMotorSpeed + g) );

  Serial.println( "tape 1: " + String( analogRead(FRONT_TAPE_SENSOR_1) ));
  Serial.println( "tape 2: " + String( analogRead(FRONT_TAPE_SENSOR_2 ) ));

  Serial.println( "error: " + String( forwardError ));
  Serial.println( "p: " + String( forward_p ));

  SerialPort.println( "tape 1: " + String( analogRead(FRONT_TAPE_SENSOR_1) ));
  SerialPort.println( "tape 2: " + String( analogRead(FRONT_TAPE_SENSOR_2 ) ));

  Serial.print("g: ");
  Serial.println(forward_g);

  Serial.print("mL: ");
  Serial.println(mL);

  Serial.print("mR: ");
  Serial.println(mR);

  }

  }

  /*  Driving backwards  */

  if (true) {

    SerialPort.println("Backwards!!");

    backwardError = (double) analogRead(BACK_TAPE_SENSOR_1) - analogRead(BACK_TAPE_SENSOR_2);


    double BACKWARD_LOOP_GAIN = 1.0;
    double BACKWARD_P_GAIN = 0.55;//1.4 goes very slowl
    double BACKWARD_I_GAIN = 0.0;
    double BACKWARD_D_GAIN = 1.36;//0.9;//0.4;//0.7;//0.9;//1.8;//1.9;//2.5;//2.0;//1.8;//0.9;//0.7

    backward_p = BACKWARD_P_GAIN * backwardError;
    backward_d = BACKWARD_D_GAIN * (backwardError - backwardLastError);
    backward_i = BACKWARD_I_GAIN * backwardError + backward_i; //const * error + previous int value
    if (backward_i > max_I) {backward_i = max_I;}
    if (backward_i < -max_I) {backward_i = -max_I;}

    backward_g = BACKWARD_LOOP_GAIN * ( backward_p + backward_i + backward_d ); 
    backwardLastError = backwardError; 

    //SEND MOTOR VALS

    MotorL.backward( (backwardMidMotorSpeed - 1 * backward_g) );
    MotorR.backward(  1 / 1.2 * ( ( backwardMidMotorSpeed + 1 * backward_g) ) );


    /*  SerialPort & Serial Monitor prints  */
    {

    int mL = backwardMidMotorSpeed - backward_g;
    int mR = backwardMidMotorSpeed + backward_g;
    // SerialPort.println("g: " + String( g) );
    // SerialPort.println("m1: " + String( midMotorSpeed - g) );
    // SerialPort.println("m2: " + String( midMotorSpeed + g) );

    Serial.println( "tape 1: " + String( analogRead(FRONT_TAPE_SENSOR_1) ));
    Serial.println( "tape 2: " + String( analogRead(FRONT_TAPE_SENSOR_2 ) ));

    Serial.println( "error: " + String( backwardError ));
    Serial.println( "p: " + String( backward_p ));

    SerialPort.println( "tape 1: " + String( analogRead(FRONT_TAPE_SENSOR_1) ));
    SerialPort.println( "tape 2: " + String( analogRead(FRONT_TAPE_SENSOR_2 ) ));

    Serial.print("g: ");
    Serial.println(backward_g);

    Serial.print("mL: ");
    Serial.println(mL);

    Serial.print("mR: ");
    Serial.println(mR);

    }


  }

}

/**
 * @brief function attached to RotaryA and RotaryB to update encoder values
 * 
 */
void ISRUpdateEncoder(){

  bool A = digitalRead(ROTARY_A);
  bool B = digitalRead(ROTARY_B);

  encoder1.updateEncoder(A, B);
  encoder1.updateTime( millis() );

}


/**
 * @brief function for reading the debounced button press values.
 * 
 */
void ISRButton() {

  //  Serial.print("inside the interrupt");

  encoder1.resetIncrement();
  //delay(100);
  buttonPressed = true;

  //MotorL.buttonPressed = true;

}

/**
 * @brief function for handling station detection on sensor B on left side
 * 
 */
void tapeDetectedLB() {

  SerialPort.println( "tape LA: " + String( analogRead(TAPE_LA) ));
  SerialPort.println( "tape LB: " + String( analogRead(TAPE_LB) ));

  // only stop if driving in right direction, reached the target destination, and not already stopped
  if (!stopped && !direction && currentStation == (targetStation - 1)) {
    MotorL.stop();
    MotorR.stop();
    currentStation++;
    stopped = true;
    delay(1000);
  }
  else if (stopped) {
    stopped = false;
  }

}

/**
 * @brief function for handling station detection on sensor A on left side
 * 
 */
void tapeDetectedLA() {

  SerialPort.println( "tape LA: " + String( analogRead(TAPE_LA) ));
  SerialPort.println( "tape LB: " + String( analogRead(TAPE_LB) ));

  // only stop if driving in right direction, reached the target destination, and not already stopped
  if (!stopped && direction && currentStation == (targetStation + 1)) {
    MotorL.stop();
    MotorR.stop();
    currentStation--;
    stopped = true;
    delay(1000);
  }
  else if (stopped) {
    stopped = false;
  }

}