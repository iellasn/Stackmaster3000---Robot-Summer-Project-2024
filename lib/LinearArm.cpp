#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include <HardwareSerial.h>

#include "RotaryEncoder.h"
#include "Motor.h"
#include "robotConstants.h"

/*  BP pin defs for UART */

#define RX PB11
#define TX PB10

HardwareSerial SerialPort(USART3);
String msg;


/*  Function Declerations  */
void ISRUpdateLinearArmEncoder();
void ISRButton();
void localize();


/*  PID Control Values  */

int setVal = 32;

int measuredVal;

double armError = 0.0;
double lastArmError = 0.0;

double MAX_I = 140;

double p_arm_Val, d_arm_Val, i_arm_Val;

double g_arm_Val ;


/*  Object declerations  */

encoder::RotaryEncoder armEncoder(PB_8, PB_9);
movement::Motor armMotor(ARM_MOTOR_P1, ARM_MOTOR_P2, &armEncoder);

void setup() {

  /*  Setup Serials  */
  {
    /*  Setup Serial Monitor  */
    Serial.begin(115200);
    Serial.println("Hello" + String(BOARD_NAME));

    /*  Setup UART port  */
    SerialPort.begin(115200);

  }

  /*  PinModes  */
  {
    /*  Motor Pins  */
    pinMode(armMotor.getPinA(), OUTPUT);
    pinMode(armMotor.getPinB(), OUTPUT);

    /*  Encoders  */
    pinMode(armEncoder.getPinA(), INPUT);
    pinMode(armEncoder.getPinB(), INPUT);

  }

  /*  Attach Interrupts  */
  {
    /*  Arm encoders  */
    attachInterrupt(digitalPinToInterrupt(armEncoder.getPinA()), ISRUpdateLinearArmEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(armEncoder.getPinB()), ISRUpdateLinearArmEncoder, CHANGE);
  }

  /*  Perform Setup Actions  */
  {

    armMotor.off();
    delay(500);
    armMotor.forward(3000);
    delay(100);
    armMotor.off();
    delay(100);

    /*  perform motor sweep to initialize motion  */
    localize();

  }

}


void loop() {

  int setVal = 32;
  int measuredVal;


  int readVal = SerialPort.read();

  setVal = map(readVal, 0, 1023, -500, 500);

  measuredVal = armMotor.encoder->getIncrements();

  armError = setVal - measuredVal;
  

  double PLATE_PID_TOTAL_GAIN = 1.0;
  double P_ARM_GAIN = 0.55;//1.4 goes very slowl
  double I_ARM_GAIN = 0.0;
  double D_ARM_GAIN = 0;

  p_arm_Val = P_ARM_GAIN *armError;
  d_arm_Val = D_ARM_GAIN * (armError - lastArmError);
  i_arm_Val = I_ARM_GAIN *armError + i_arm_Val; //const *armError + previous int value
  if (i_arm_Val > MAX_I) {i_arm_Val = MAX_I;}
  if (i_arm_Val < -MAX_I) {i_arm_Val = -MAX_I;}

  g_arm_Val = PLATE_PID_TOTAL_GAIN * ( p_arm_Val + i_arm_Val + d_arm_Val ); 
  lastArmError =armError; 

// PID hopefully
  armMotor.forward( g_arm_Val );

}


/**
 * @brief function attached to RotaryA and RotaryB to update encoder values
 * 
 */
void ISRUpdateLinearArmEncoder(){

  bool A = digitalRead( armEncoder.getPinA() );
  bool B = digitalRead( armEncoder.getPinB() );

  armEncoder.updateEncoder(A, B);
  armEncoder.updateTime( millis() );

}


/**
 * @brief performs motor sweep to localize range of elevator motion 
 * updates encoder range values and sends the motor to the center once completed
 * 
 */
void localize() {

    int bottom;
    int top;
    int center;

    // turn motor until elevator reaches bottom limit
    while (!digitalRead(ARM_LOWER_LIMIT_SWITCH)) {
        armMotor.backward(2000);
    }

    // initialize bottom of elevator movement
    armMotor.off();
    armMotor.encoder->resetIncrement();
    bottom = armMotor.encoder->getIncrements();

    // turn motor in opposite direction until top limit reached
    while (!digitalRead(ARM_UPPER_LIMIT_SWITCH)) {
        armMotor.forward(2000);
    }

    // initialize top of elevator movement
    armMotor.off();
    top = armMotor.encoder->getIncrements();
    armMotor.encoder->setMaxIncrement(top);

    // turn motor and reach middle of motion
    center = top / 2;
    while (armMotor.encoder->getIncrements() != center) {
        armMotor.backward(3000);
    }
    armMotor.stop();

}