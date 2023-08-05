# EK-TM4C123GXL

![alt text](ek-tm4c123gxl.png)

## Debugging with JLink SWD

1. Solder a strip header to DBG 
2. Move switch to position DEVICE (Avoid conflicts to TI-ICDI)
3. Connect pin EXTDBG to ground (disable TI-ICDI)
4. Connect +3V3 to jtag pin 1
5. Connect GND to jtag pin 4
6. Connect TCK to jtag pin 9 (SWCLK)
7. Connect TMS to jtag pin 7 (SWDIO)
8. Connect RESET to jtag pin 15 (nRST)

## Extra changes

1. Remove resistors R9 and R10.
2. Solder resistors R25 and R29 (0 ohm)