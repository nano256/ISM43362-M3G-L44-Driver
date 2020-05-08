# ISM43362-M3G-L44-Driver
A Driver for the ISM43362-M3G-L44 WiFi Module made for STM32. The driver provides an easy to understand interface which replaces the need to control the Wifi module with the AT command set. 

This driver has no dependency on RTOS or Arm Mbed OS and is loosely bound to the STM HAL library. All plattform dependent interfaces are isolated and can be changed effortlessly for uncomplicated porting to other systems working with C.

## Usage in STM32
- Copy `wifi.h` in the `inc` folder and `wifi.c` in your `src` folder of your project.
- Add `#include "wifi.h"` in whichever file you want to use the Wifi module in.
