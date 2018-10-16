/*! \file animation.c

  \brief Drawing functions based on the time
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"

void delay(unsigned len)
{
	while(len)
	{
		asm(""); // ensure that the loop executes
		len--;
	}
}


// In normal operation there are four LEDs powered on
// The minute is full bright
// The hour is half bright
// the second is quarter bright
// the previous second fades
uint8_t led_display[] = {};
uint8_t led_bright[] = {};

void led_draw()
{
	int i;
	for(i=0 ; i < NUM_DISPLAY ; i++)
	{
		led_on(led_display[i]);
		delay(led_bright[i]);
	}

	led_off();
}


void animation_draw()
{
	static int oldsec;
	static int do_second_animation = 0;
	static int do_minute_animation = 0;
	static int do_hour_animation = 0;
	static int do_sparkle_animation = 0;
	static int hour_dir;
	static unsigned hour_bright = 0;

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
			led_display[1] = 60 + (led_display[1] - 60 + 1 + 12) % 12;
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
}
