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

//! Power On Self Test
static int
post(void)
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

#if 1
	rtc_init();
	ucs_init(); // doesn't work if crystal isn't there?

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
	static int oldsec;
	static int do_animation = 0;
	static int hour_dir = +1;
	static unsigned hour_bright = 255;

	// cycle through the LEDs once per second
	if (oldsec != RTCSEC)
	{
		oldsec = RTCSEC;
		const int i = oldsec % 6;

		led_display[0] = RTCMIN;
		led_display[1] = 60 + (RTCHOUR % 12);
		led_display[2] = RTCSEC;

		if (RTCMIN == RTCSEC)
			do_animation = 60;
	} else
	if (do_animation)
	{
		led_display[2] = (led_display[2] + 1) % 60;
		do_animation--;
	}

	// make the hour "breath"
	if (hour_bright == 255)
		hour_dir = -1;
	else
	if (hour_bright == 0)
		hour_dir = +1;

	hour_bright += hour_dir;

	// set the default brightnesses for minute and second
	led_bright[0] = 32;
	led_bright[1] = hour_bright / 8;
	led_bright[2] = 0;

	led_draw();
}
