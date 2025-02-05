# Team-6---Robot-Summer



bottom -262
top 0











Linear arm has 160 encoder ticks between the limit switches. extended is -160, closed is 0


Elevator has 515 encoder ticks between the limit switches. Top is 0, bottom is -515.



elevator calib:

bottom limit: -15
claw heighy: ~241 to 267
forklift height: ~bottom of limit switch 



Tasks to do:

PID control of rod with limit switches (needed for:)
    Elevator system movement
    Plate serving movement
    rack and pinion 
    Lazy susan rotation code (the same type of abstraction. I think identical code as above)


Claw servo integration with limit switches

DONE: drive PID tuning
    find better suited motors


```
├── README.md
├── include
│   └── README
├── lib
│   ├── BPmain.cpp
│   ├── Motor
│   │   ├── Motor.cpp
│   │   └── Motor.h
│   ├── README
│   ├── RobotConstants
│   │   ├── bpConstants.h
│   │   ├── espConstants.h
│   │   └── robotConstants.h
│   ├── RobotSystems
│   │   ├── RobotSystems.cpp
│   │   └── RobotSystems.h
│   ├── RotaryEncoder
│   │   ├── RotaryEncoder.cpp
│   │   └── RotaryEncoder.h
│   ├── bp-localize-tests.cpp
│   ├── bp-plate.cpp
│   ├── bpplatformio.ini
│   ├── claw
│   │   ├── claw.cpp
│   │   └── claw.h
│   ├── esp-Main-Final copy.cpp
│   ├── esp-Main-Final.cpp
│   ├── esp-localization-test.cpp
│   ├── esp-servo-tests.cpp
│   ├── espMain.cpp
│   ├── files
│   │   ├── Elevator.cpp
│   │   ├── IR_Sensor.cpp
│   │   ├── LinearArm.cpp
│   │   ├── PlateMovement.cpp
│   │   ├── Station_Transversing.cpp
│   │   ├── lazySusan.cpp
│   │   ├── linear arm update.cpp
│   │   └── main.cpp
│   ├── func
│   │   └── func.h
│   ├── pwm test.cpp
│   ├── tapeTest.cpp
│   └── uartTests
│       ├── BPmain-uartTests.cpp
│       ├── espDisplay.cpp
│       └── espMain-uartTest.cpp
├── platformio.ini
├── src
│   └── bp-Main-Final.cpp
└── test
    └── README
```


Determine communcation data structure & what information to send

PID tuning for IR beacon sensor

start to combine code components 
