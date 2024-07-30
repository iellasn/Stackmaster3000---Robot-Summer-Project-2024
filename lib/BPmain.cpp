
// #ifndef ESP32

#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>


/*  libraries we wrote  */
#include <Motor.h>
#include <RotaryEncoder.h>
#include <RobotSystems.h>
//#include <robotConstants.h>
#include <bpConstants.h>

#define TAPE_THRESHOLD 800 

//Encoder values for different heights of the elevator 
#define FORKLIFT_COUNTER_HEIGHT 1000
#define FORKLIFT_SECURE_HEIGHT 1001 //A height that is just a little bit taller than counter height
#define CLAW_COUNTER_HEIGHT 900
#define CLAW_SECURE_HEIGHT 901

void isrUpdateElevatorEncoder();


/*  Object declerations  */

movement::Motor motorL(MOTOR_L_P1, MOTOR_L_P2);
movement::Motor motorR(MOTOR_R_P1, MOTOR_R_P2);

encoder::RotaryEncoder elevatorEncoder(ELEVATOR_ENCODER_PA, ELEVATOR_ENCODER_PB);
movement::EncodedMotor ElevatorMotor(ELEVATOR_P1, ELEVATOR_P2, &elevatorEncoder);

//robot::RobotSubSystem Elevator();
robot::RobotSubSystem ElevatorSystem(ELEVATOR_LIMIT_BOTTOM, ELEVATOR_LIMIT_TOP, &ElevatorMotor);

robot::DrivePID 
driveSystem(TAPE_SENSOR_FORWARD_2, TAPE_SENSOR_FORWARD_1, TAPE_SENSOR_BACKWARD_1, TAPE_SENSOR_BACKWARD_2, &motorL, &motorR); 


HardwareSerial SerialPort(USART3);

enum State{
    START, 
    TRANSITION_TO_4,
    PROCESS_STATION_4,
    PROCESS_STATION_5, 
    TRANSITION_TO_6,
    PROCESS_STATION_6,
    TRANSITION_TO_5,
    TRANSITION_TO_62,
    PROCESS_STATION_62,
    IDLE,
    FINISHED
};
State currentState = IDLE;


void setup() {

    delay(2000);

    Serial.begin(115200);
    Serial.println("setup");
    SerialPort.begin(115200);


    pinMode(elevatorEncoder.getPinA(), INPUT_PULLUP);
    pinMode(elevatorEncoder.getPinB(), INPUT_PULLUP);

    pinMode(motorL.getPinA(), OUTPUT);
    pinMode(motorL.getPinB(), OUTPUT);

    pinMode(motorR.getPinA(), OUTPUT);
    pinMode(motorR.getPinB(), OUTPUT);

    pinMode(ElevatorMotor.getPinA(), OUTPUT);
    pinMode(ElevatorMotor.getPinB(), OUTPUT);
    

    attachInterrupt(digitalPinToInterrupt(elevatorEncoder.getPinA()), isrUpdateElevatorEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(elevatorEncoder.getPinB()), isrUpdateElevatorEncoder, CHANGE);



    // ElevatorSystem.localize();

}



void loop(){


switch (currentState){
case START: 
    delay(1000); 
    currentState = TRANSITION_TO_4; 
    break;

case TRANSITION_TO_4:
    bool stopConditionsMet() {
        return lineCountRight() >= 2 || lineCountLeft() >= 4; 
    }

    while(!stopConditionsMet()){
        pidDriving(); // to the right 
        driveSystem.updateForwardDrivePID();
    }

    motorL.stop();
    motorR.stop(); 

    SerialPort.println(1); 

    currentState = PROCESS_STATION_4;
    break; 

case PROCESS_STATION_4:
if( SerialPort.available() ){
    int receivedVal = SerialPort.parseInt(); 
    if(receivedVal == 2){
        ElevatorSystem.moveToValue(FORKLIFT_COUNTER_HEIGHT); 
        SerialPort.println(3); 
    }
} 
if( SerialPort.available() ){
    int receivedVal = SerialPort.parseInt(); 
    if(receivedVal == 4){
    ElevatorSystem.moveToValue(FORKLIFT_SECURE_HEIGHT);
    }
} currentState = TRANSITION_TO_6; 
    // Serial.println("enc: " + String(    elevatorEncoder.getIncrements() ) );
    // // ElevatorSystem.updatePID(80);
case TRANSITION_TO_6:
    
    driveSystem.updateBackwardDrivePID();
    //pidDriving(); //to the left
    //stopping at Serving area; 
    //once stopped Serial.println(1);
    if(SerialPort.available()){
        int receivedVal = SerialPort.parseInt(); 
        if(receivedVal == 2){
            currentState = PROCESS_STATION_6; 
        }
    }
case PROCESS_STATION_6:
    ElevatorSystem.moveToValue(FORKLIFT_COUNTER_HEIGHT);
    Serial.println(3);
    if(SerialPort.available()){
        int receivedVal = SerialPort.parseInt();
        if(receivedVal = 4){
            currentState = TRANSITION_TO_5; 
        }
    }
case TRANSITION_TO_5:
bool stopConditionsMet() {
    return lineCountRight() >= 1 || lineCountLeft() >= 2; 
}
while(!stopConditionsMet){
    pidDriving(); //to the left
}
motorL.stop();
motorR.stop(); 
SerialPort.println(1);

if(SerialPort.available()){
    int receivedVal = SerialPort.parseInt(); 
    if(receivedVal == 2){
        currentState = PROCESS_STATION_5;
    }
}
case PROCESS_STATION_5:
    ElevatorSystem.moveToValue(CLAW_COUNTER_HEIGHT);
    SerialPort.println(3);

    if(SerialPort.available()){
        int receivedVal = SerialPort.parseInt(); 
        if(receivedVal == 5){
            ElevatorSystem.moveToValue(CLAW_SECURE_HEIGHT);
            currentState = TRANSITION_TO_62;
        }
    }
case TRANSITION_TO_62:
    
    driveSystem.updateForwardDrivePID();
    //pidDriving(); //to the right
    //stopping at Serving area; 
    //once stopped Serial.println(1);
     if(SerialPort.available()){
        int receivedVal = SerialPort.parseInt(); 
        if(receivedVal == 6)
        currentState = FINISHED; 
    }
case FINISHED: //aka transition to 4.2 
    bool stopConditionsMet() {
        return lineCountRight() >= 1 || lineCountLeft() >= 2; 
    }
    while(!stopCondiionsMet){
        //pidDriving();
        driveSystem.updateForwardDrivePID(); 
    }
    motorL.stop();
    motorR.stop(); 
    SerialPort.println(1);
    currentState = PROCESS_STATION_4; 
}


}

bool markerDetected(){
    return (analogRead(TAPE_SENSOR_LEFT_1) >= TAPE_THRESHOLD || analogRead(TAPE_SENSOR_RIGHT_1) >= TAPE_THRESHOLD)
}

int lineCountLeft(){
    int count = 0; 
    if(analogRead(TAPE_SENSOR_LEFT_1) >= TAPE_THRESHOLD){
        count++; 
    }
    return count; 
}

int lineCountRight(){
    int count = 0; 
    if(analogRead(TAPE_SENSOR_RIGHT_1) >= TAPE_THRESHOLD){
        count++; 
    }
    return count; 
}

void isrUpdateElevatorEncoder(){

    bool A = digitalRead(elevatorEncoder.getPinA());
    bool B = digitalRead(elevatorEncoder.getPinA());
    elevatorEncoder.updateEncoder(A, B);

}

// #endif