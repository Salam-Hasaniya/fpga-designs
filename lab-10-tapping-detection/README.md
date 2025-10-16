# Lab 10 – Tapping Detection Using an Accelerometer

## Overview

This lab uses an accelerometer to detect tapping events based on sudden spikes in acceleration. The goal is to use the intensity of a tap as an input to control how many onboard LEDs are lit.

When an object is tapped, the acceleration changes rapidly, creating detectable "spikes" in the sensor data. These spikes can be analyzed to classify the tap's strength and trigger LED output accordingly.

### Objectives

- Derive a program to capture real-time acceleration values during tapping events.
- Use UART to send accelerometer readings for analysis.
- Record and identify patterns in x, y, and z axis readings during taps.
- Create logic that maps tap intensity to LED output:
  - A **light tap** lights up 1 LED (LED0).
  - A **medium tap** lights up a few LEDs (e.g., LED0–LED14).
  - A **strong tap** lights up **all 16 LEDs** (LED0–LED15).
