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
#include "ucs.h"
#include "led.h"
#include "power.h"
#include "stopwatch.h"


//! Main method.
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// drive port J and 1 to ground to avoid CMOS power drain
	PJDIR |=  0xF;
	PJOUT &= ~0xF;
	P1DIR |=  0xF;
	P1OUT &= ~0xF;

	// turn off all the LEDs to reduce power
	led_off();

#if 1
	rtc_init();
	ucs_init(); // doesn't work if crystal isn't there?

	// Setup and enable WDT 16ms, ACLK, interval timer
	WDTCTL = WDT_ADLY_16;
	SFRIE1 |= WDTIE;

	// go into low power mode
	power_setvcore(0);
	ucs_slow();

	__bis_SR_register(LPM3_bits + GIE);        // Enter LPM3

	// flash the 0 hour so that we know something is wrong
	while(1)
	{
		led_on(60); delay(30000);
		led_off(); delay(30000);
		led_on(60); delay(30000);
		led_off(); delay(60000);
	}
#else
	// test pattern for all LEDs
	while(1)
	{
		int i, j;
		for(i = 0 ; i < 72 ; i++)
		{
			for(j = 0 ; j < 20 ; j++)
			{
				led_on(i); delay(10);
				led_off(); delay(1000);
			}
		}

		delay(10000);
	}
#endif
}


//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR)))
watchdog_timer(void)
{
	ucs_fast();
	//animation_draw();
	stopwatch_draw();
	led_off();
	ucs_slow();
}
