# Midterm Project: Ambient Light Sensor using PmodALS
This project implements SPI communication between the Nexys A7 FPGA and the PmodALS (Ambient Light Sensor) to detect and respond to changes in light intensity. It uses a custom SPI controller in Verilog and corresponding C++ driver code to interface with the sensor. Depending on the light level, a number of LEDs will light up on the board.

## Components Used
- Nexys A7 FPGA Board  
- PmodALS (Ambient Light Sensor, using ADC081S021 over SPI)
- Verilog (for hardware logic)
- C++ (for software driver using MMIO)
- Vivado + Vitis
- LEDs(for application display)
