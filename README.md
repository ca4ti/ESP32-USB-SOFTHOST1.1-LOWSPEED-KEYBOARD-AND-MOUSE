# ESP32-USB-SOFTHOST1.1-LOWSPEED-KEYBOARD-AND-MOUSE
Another keyboard and mouse parser for USB1.1 soft host low speed devices for ESP32 for Arduino IDE, with debugging . 
It can parse USB keyboard scancodes to chars (keys) also mouse position... 

(nothing guaranteed)

![Image description](https://github.com/nathalis/ESP32-USB-SOFTHOST1.1-LOWSPEED-KEYBOARD-AND-MOUSE/blob/main/US_PC_keyboard_keycodes.png)

USB 1.1 has 2 modes:
--------------------
LOW SPEED = 1.5Mbit/s (supported)
FULL SPEED = 12Mbit/s (not supported yet)

### connection:

![Image description](https://github.com/nathalis/ESP32-USB-SOFTHOST1.1-LOWSPEED-KEYBOARD-AND-MOUSE/blob/main/connection.png)

or you can change GPIO > 7 and GPIO < 31, must be INPUT/OUTPUT...

### future idea:

...for USB 1.1 full speed soft host solution is needed 12MHz I/O (maybe I2S = max. 20MHz or so... )

Source projects:
https://github.com/tobozo/ESP32-USB-Soft-Host
https://github.com/sdima1357/esp32_usb_soft_host

