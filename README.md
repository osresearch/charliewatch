![Watch rendering](watch.png)

"Analog" watch with 60+12 Charlieplexed LEDs. Based on Travis Goodspeed's
GoodWatch21 schematic, using the CC430F5137IRGZR CPU, which doesn't
have the LCD controller of the CC430F6xxx in the GoodWatch. There are
also pads for a six pin FTDI cable with pogopins for programming or
serial communication.

v0.1 was ordered 2018-09-27, not yet tested.
[Charliewtch schematic](datasheets/charliewatch.pdf)

Programming cable
-----

From the top of the board:
* RST / TDIO
* RXD on programmer
* TXD on programmer
* VCC
* TST / TCK
* GND

There is also a four-wire setup that might simplify the wiring.
Travis documents it and the power consumption here:
https://github.com/travisgoodspeed/goodwatch/wiki/EnergyTrace

Bricked Launchpad
-----

If you brick your launchpad by running Energia under Linux, it will no
longer program the Charliewatch.  The other weirdness is that `tilib`
can detect the device, but can't write to it unless run as root.

Energy Trace
----
* `libmsp430.so`: https://dlbeer.co.nz/articles/slac460y/index.html
* https://github.com/carrotIndustries/energytrace-util
* Have to update path to `MSP430.h` from the `slac/DLL430_v3/include` directory


