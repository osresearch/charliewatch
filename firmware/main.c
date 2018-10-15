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
	static int oldsec;
	static int do_second_animation = 0;
	static int do_minute_animation = 0;
	static int do_hour_animation = 0;
	static int do_sparkle_animation = 0;
	static int hour_dir;
	static unsigned hour_bright = 0;

	ucs_fast();

	// update the LEDs once per second
	if (oldsec != RTCSEC
	&& !do_minute_animation
	&& !do_second_animation
	&& !do_sparkle_animation)
	{
		oldsec = RTCSEC;

		led_display[0] = RTCMIN;
		led_display[1] = 60 + (RTCHOUR % 12);
		led_display[2] = RTCSEC;

		const unsigned hour_five = 5 * (RTCHOUR % 12);

		if (RTCMIN == RTCSEC && RTCMIN == hour_five)
		{
			// when the hour, minute and second hands line up
			do_sparkle_animation = 240;
		} else
		if (RTCSEC == 0 && (RTCMIN % 5) == 0)
		{
			// every five minutes
			// wrap the second hand twice
			// and the minute hand at half speed.
			do_minute_animation = 120;
			do_second_animation = 120;
		} else
		if (RTCMIN == RTCSEC)
		{
			// when the second hand hits the minute hand,
			// animate the second hand for one revolution
			do_second_animation = 60;
		} else
		if (RTCSEC == hour_five)
		{
			do_hour_animation = 12 * 4;
		}
	}

	if (do_second_animation)
	{
		// run the second hand forwards at a fast speed
		led_display[2] = (led_display[2] + 1) % 60;
		do_second_animation--;
	}
	if (do_minute_animation)
	{
		// run the minute hand backwards at half speed
		if (0 == (do_minute_animation & 1))
			led_display[0] = (led_display[0] + 60 - 1) % 60;
		do_minute_animation--;
	}
	if (do_hour_animation)
	{
		if (0 == (do_hour_animation & 3))
			led_display[1] = 60 + (led_display[1] - 60 - 1 + 12) % 12;
		do_hour_animation--;
	}
	if (do_sparkle_animation)
	{
		led_display[0] = rand() % 60;
		led_display[1] = 60 + rand() % 12;
		led_display[2] = rand() % 60;
		do_sparkle_animation--;
	}

	// make the hour "breath"
	if (hour_bright == 32*4)
		hour_dir = -1;
	else
	if (hour_bright == 0)
		hour_dir = +1;

	hour_bright += hour_dir;

	// set the default brightnesses for minute and second
	led_bright[0] = 48;
	led_bright[1] = hour_bright / 4;
	led_bright[2] = 0;

	led_draw();
	ucs_slow();
}
