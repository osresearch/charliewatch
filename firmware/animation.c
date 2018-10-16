/*! \file animation.c

  \brief Drawing functions based on the time.

Animations are drawn:
* When the second hand hits the minute hand
* When the second hand hits the minute hand on a 5 minut emark
* When the second hand hits the hour hand
* When all three hands line up
* At midnight/noon
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


void sparkle_animation(unsigned count)
{
	// should make the sparkles converge inward
	(void) count;
	led_on(rand() % 60);
	led_on(rand() % 60);
	led_on(rand() % 60);
	led_on(60 + (rand() % 12));
}

// animation for when all the hands align
void align_animation(unsigned count)
{
	unsigned i;

	led_on(RTCSEC);

	if (count > 60)
	{
		// draw out from the minute up to the count
		for(i=0 ; i < 120 - count ; i++)
			led_on((RTCMIN + i) % 60);

		// draw out from the hour hand in reverse
		for(i=0 ; i < (120 - count)/5 ; i ++)
			led_on(60 + (RTCHOUR + 12 - i) % 12);
	} else {
		// start collapsing back to the minute
		for(i= 0 ; i < count ; i++)
			led_on((RTCMIN - i + 60) % 60);

		// collapse back to the hour hand
		for(i=0 ; i < count/5 ; i ++)
			led_on(60 + (RTCHOUR + i) % 12);
	}
}

void hour_animation(unsigned count)
{
	led_display[2] = RTCSEC;

	// run the hour forward
	if (0 == (count % 5))
		led_display[1] = 60 + (led_display[1] - 60 + 1 + 12) % 12;

	led_draw();
}


void minute_animation(unsigned count)
{
	// run the minute hand backwards at half speed
	if (0 == (count & 1))
		led_display[0] = (led_display[0] + 60 - 1) % 60;

	// run the second hand forward at full speed
	led_display[2] = (led_display[2] + 1) % 60;

	led_draw();
}


void second_animation(unsigned count)
{
	// run the second hand forward at full speed
	led_display[2] = (led_display[2] + 1) % 60;

	led_draw();
}

void animation_draw()
{
	static int oldsec;
	static int do_second_animation = 0;
	static int do_minute_animation = 0;
	static int do_hour_animation = 0;
	static int do_align_animation = 0;
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

		if (RTCMIN == 0 && RTCSEC == 0 && hour_five == 0)
		{
			// midnight or noon, draw the sparkles
			do_sparkle_animation = 240;
		} else
		if (RTCMIN == RTCSEC && RTCMIN == hour_five)
		{
			// when the hour, minute and second hands line up
			do_align_animation = 120;
		} else
		if (RTCSEC == RTCMIN && (RTCMIN % 5) == 0)
		{
			// when the minute and second line up
			// every five minutes
			// wrap the second hand twice
			// and the minute hand at half speed.
			do_minute_animation = 120;
		} else
		if (RTCMIN == RTCSEC)
		{
			// when the second hand hits the minute hand,
			// run the second hand forwards
			do_second_animation = 60;
		} else
		if (RTCSEC == hour_five)
		{
			// second hand aligns with the hour hand
			do_hour_animation = 60;
		}
	}

	if (do_sparkle_animation)
	{
		sparkle_animation(do_sparkle_animation--);
		return;
	}

	if (do_align_animation)
	{
		align_animation(do_align_animation--);
		return;
	}

	if (do_hour_animation)
	{
		hour_animation(do_hour_animation--);
		return;
	}

	if (do_minute_animation)
	{
		minute_animation(do_minute_animation--);
		return;
	}

	if (do_second_animation)
	{
		second_animation(do_second_animation--);
		return;
	}

	// make the hour "breath"
	if (hour_bright == 32*4)
		hour_dir = -1;
	else
	if (hour_bright == 0)
		hour_dir = +1;

	hour_bright += hour_dir;

	// set the default brightnesses for minute and second
	led_bright[0] = 32;
	led_bright[1] = (hour_bright / 4);
	led_bright[2] = 0;

	led_draw();
}
