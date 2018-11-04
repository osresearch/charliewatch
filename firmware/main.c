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
#include "button.h"


//! Main method.
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// drive port J and 1 to ground to avoid CMOS power drain
	PJDIR |=  0xF;
	PJOUT &= ~0xF;
	P1DIR |=  0xF;
	P1OUT &= ~0xF;


	button_init();

	// turn off all the LEDs to reduce power
	led_off();


#if 1
	led_test();
#else
	while(1) {
		led_test();
		delay(1000);
	}
#endif

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
		led_on(60); delay(1);
		led_off(); delay(3);
		led_on(60); delay(1);
		led_off(); delay(6);
	}
}


extern void stopwatch_draw(void);
extern void clockset_draw(void);
extern void animation_draw(void);

static void (*const modes[])(void) = {
	animation_draw,
	stopwatch_draw,
	clockset_draw,
	clockset_draw,
};

static const unsigned mode_count = sizeof(modes) / sizeof(*modes);
unsigned watch_mode = 0;


//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR)))
watchdog_timer(void)
{
	ucs_fast();

	button_update();

	// long hold advances mode
	if (button_long)
		watch_mode = (watch_mode + 1) % mode_count;

	modes[watch_mode]();

	led_off();
	ucs_slow();
}
