/*! \file main.c

  \brief Main module.  This version initializes the hardware and then
   drops to a low power mode, letting the WDT update the LEDs every 16ms

  Schematic and more at https://trmm.net/Charliewatch
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rtc.h"
#include "led.h"

//! Power On Self Test
int post(void)
{
	return 0;
}


//! Main method.
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// drive port J to ground to avoid CMOS power drain
	PJDIR |=  0xF;
	PJOUT &= ~0xF;

	rtc_init();

	// Setup and enable WDT 16ms, ACLK, interval timer
	WDTCTL = WDT_ADLY_16;
	SFRIE1 |= WDTIE;

	__bis_SR_register(LPM3_bits + GIE);        // Enter LPM3

	// flash the 0 hour so that we know something is wrong
	while(1)
	{
		led_on(60); delay(30000);
		led_off(); delay(30000);
		led_on(60); delay(30000);
		led_off(); delay(60000);
	}
}


//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR)))
watchdog_timer(void)
{
	static int oldsec;

	// cycle through the LEDs once per second
	if (oldsec != RTCSEC)
	{
		oldsec = RTCSEC;
		const int i = oldsec % 6;

		led_display[0] = 60 + ((i + 0) % 6);
		led_display[1] = 60 + ((i + 1) % 6);
		led_display[2] = 60 + ((i + 2) % 6);
		led_display[3] = 60 + ((i + 3) % 6);
	}

	led_draw();
}
