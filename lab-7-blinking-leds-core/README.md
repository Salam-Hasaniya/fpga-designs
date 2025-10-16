# Lab 7: Blinker Core

## Hardware Changes

- **Updated `chu_io_map.svh`**
  - Defined `S4_BLINKER` for Slot 4

- **Added `blinker_slot4.sv`**
  - Implements independent blinking for 4 LEDs

- **Updated `mmio_sys_vanilla.sv`**
  - Instantiated `blinker_slot4` in **Slot 4**
  - Slot 3 (`GPI`) left untouched
  - LEDs are now **exclusively controlled** by Slot 4

---

## Driver and Application Changes

- **Added Slot 4 `BlinkerCore` Support**

- **Updated `gpio_cores.h` and `gpio_cores.cpp`**
  - Created `BlinkerCore` class
  - Implements `set_blink_rate()` for each LED

- **Modified `chu_io_map.h`**
  - Defined `S4_BLINKER` to match hardware

- **Updated Main Test Program**
  - Instantiated `BlinkerCore`
  - Added `blinker_test()` to configure LED blink rates

