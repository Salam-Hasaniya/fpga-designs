# Lock System Controller

## Summary
This FPGA project implements a lock system using keyboard and joystick input, VGA display, and motor control. 
Users can set, update, and verify a combination to lock or unlock a stepper motor.

## Features
- Joystick (SPI): Enter combo using U/D/L/R directions
- PS/2 Keyboard: Menu navigation and input
- VGA OSD: Displays menu, combo status, and system feedback
- Motor Control: Simulates lock/unlock with a stepper motor
- Combo System:
  - Set combo length (4–9)
  - Define and update combo
  - Unlock only if input matches stored combo

## Menu Options
1: Set combo length and define combo  
2: Set new combination  
3: Lock or unlock system

## Hardware Requirements
- CHU Board
- PS/2 Keyboard
- Pmod Joystick (SPI)
- Stepper Motor connected via GPO
- VGA Monitor for output
