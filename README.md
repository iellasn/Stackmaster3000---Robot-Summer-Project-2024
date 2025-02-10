# Stackmaster3000 - Robot Summer Project 2024

## Overview
Stackmaster3000 is an autonomous robot control system built for the 2024 Robot Summer Competition. The system is implemented on 2 STM32F103C8 (BluePill) microcontrollers with capabilities for precise motor control, position tracking, and line following.

## Hardware
- **Microcontroller**: STM32F103C8 (BluePill)
- **Motors**: 
  - 2x Drive motors with differential drive
  - 3x Motors with encoder feedback
- **Sensors**:
  - Quadrature encoder for position tracking
  - Multiple line sensors for navigation
  - Limit switches for system calibration
- **Communication**: UART interface

## Software Architecture

### Core Components

1. **Motor Control System**
- Base motor control with PWM
- Encoded motor support with position feedback
- Emergency stop functionality

2. **Encoder System**
- Real-time quadrature decoding
- Position tracking
- Interrupt-based updates

3. **Line Following System**
- Dual sensor input
- PID-based control
- Debounced line detection

4. **State Machine**
- Multiple operational states
- Transition management
- Error handling

### Key Features

1. **Position Control**
- PID-based position control
- Automatic calibration
- Limit switch safety

2. **Line Following**
- Differential drive control
- Real-time sensor processing
- Navigation markers

3. **Hardware Abstraction**
- Platform-independent design
- Clean interface separation
- Modular component structure

### Configuration
The system can be configured through:
- `platformio.ini` for build settings
- `lib/RobotConstants/` for system constants
- `lib/RobotSystems/` for control parameters

