/*! \file Stopwatch functions

  \brief Implement a timer function on the LEDs.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"

static unsigned running = 1;
static uint32_t counter; // in ms


static void button_update(void)
{
	// check the button status
	static unsigned button;

	// if button is pressed, increment hold counter
	if ((P1IN & (1 << 5)) == 0)
	{
		button++;

		// check for long hold
		return;
	}

	// button has been released; ignore if too short
	if (button < 5)
	{
		button = 0;
		return;
	}

	// held and released for the right amount of time
	button = 0;

	// if we are running, stop running
	// otherwise reset the counter
	if (running)
	{
		running = 0;
	} else {
		counter = 0;
		running = 1;
	}
}


// called every 16ms by the RTC interrupt
void stopwatch_draw()
{
	if (running)
		counter += 16;

	button_update();
	uint32_t c = counter;
	const unsigned ms = c % 1000;
	c /= 1000;
	const unsigned sec = c % 60;
	c /= 60;
	const unsigned min = c % 12; // wrap every 12 minutes

	led_on(sec); delay(2);
	led_on(min + 60);
	led_on(ms * 60 / 1000); led_off();
}
