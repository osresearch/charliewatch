/*! \file Stopwatch functions

  \brief Implement a timer function on the LEDs.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"
#include "button.h"

static unsigned running = 1;
static uint32_t counter; // in ms


// called every 16ms by the RTC interrupt
void stopwatch_draw()
{
	if (button_short)
	{
		if (running)
		{
			// stop the counter
			running = 0;
		} else {
			// reset the counter, restart it
			counter = 0;
			running = 1;
		}
	}

	if (button_long)
	{
		// we have just entered this mode
		running = 0;
		counter = 0;
	}

	if (running)
		counter += 16;

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
