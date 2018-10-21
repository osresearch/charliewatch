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
#if 0
	const uint8_t p2 = P2OUT;
	const uint8_t p3 = P3OUT;

	while(len)
	{
		// toggle the LEDs back on
		P2OUT = p2;
		P3OUT = p3;
		len--;

		// now toggle them off
		P2OUT = 0;
		P3OUT = 0;
	}
#else
	len *= 2;
	while(len)
	{
		asm("");
		len--;
	}
#endif
}


// In normal operation there are four LEDs powered on
// The minute is full bright
// The hour is half bright
// the second is quarter bright
// the previous second fades
uint8_t led_display[] = {};
uint8_t led_bright[] = {};

void led_draw(void)
{
	int i;

	for(i=0 ; i < NUM_DISPLAY ; i++)
	{
		const unsigned bright = led_bright[i];
		if (bright > 0)
			led_on(led_display[i]);

		if (bright > 1)
			delay(bright);
		else
			led_off();
	}

	led_off();
}


void sparkle_animation(unsigned count)
{
#if 0
	// should make the sparkles converge inward
	led_on((RTCSEC + rand() % (count/8) - count/16 + 240) % 60);
	led_on((RTCMIN + rand() % (count/8) - count/16 + 240) % 60);
	led_on((RTCHOUR*5 + rand() % (count/8) - count/16 + 240) % 60);
	led_on(rand() % 60);
	led_on(60 + (rand() % 12));
#else
	unsigned i;
	if (count > 72)
		count = 12;
	else
		count /= 8;

	for(i = 0 ; i < count ; i++)
		led_on(rand() % 72);

	// start turning on the hands
	if (count < 20)
	{
		led_on(led_display[0]);
		led_on(led_display[1]);
		led_on(RTCSEC);
	}
#endif
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

// rotate all three together
void triangle_animation(unsigned count)
{
	const unsigned hour = (RTCHOUR % 12) * 5;
	unsigned i;

	if (RTCSEC < RTCMIN)
	{
		// spin all three around twice
		led_on((RTCSEC + 120 - count) % 60);
		led_on((RTCMIN + 120 - count) % 60);
		led_on((hour + 120 - count) % 60);

	} else
	if (count >= 60)
	{
		// grow the bargraph between the three points
		count = (120 - count) / 3;
		for(i = 0 ; i < count ; i++)
		{
			led_on((RTCSEC + i) % 60);
			led_on((RTCMIN + i) % 60);
			led_on((hour + i) % 60);
		}
	} else
	{
		// retract the bargraph to the three
		count /= 3;
		for(i = 0 ; i < count ; i++)
		{
			led_on((RTCSEC - i + 60) % 60);
			led_on((RTCMIN - i + 60) % 60);
			led_on((hour - i + 60) % 60);
		}
	}

	// and keep the actual hour on
	led_on(led_display[1]);
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

	// if this occured at the start of a new hour,
	// run an hour hand animation, too
	if (RTCMIN == 0 && (count % 5) == 0)
		led_display[1] = 60 + (led_display[1] - 60 - 1 + 12) % 12;

	led_draw();
}


void second_animation(unsigned count)
{
	// run the second hand forward at full speed
	led_display[2] = (led_display[2] + 1) % 60;

	led_draw();
}


static int triangle_time(int a, int b, int c)
{
	if (a % 5 != 0 || b % 5 != 0)
		return 0;
	// 0, 20, 40 => 20, 40
	// 0, 40, 20 => 40, 20
	// 20, 0, 40 => -20, 20
	// 40, 0, 20 => -40, -20
	// 40, 20, 0 => -20, -40,
	// 20, 40, 0 => 20, -20

	int d1 = b - a;
	int d2 = c - a;
	if (d1 == 20 && d2 == 40)
		return 1;
	if (d1 == 40 && d2 == 20)
		return 1;
	if (d1 == -20 && d2 == -40)
		return 1;
	if (d1 == -40 && d2 == -20)
		return 1;
	if (d1 == -20 && d2 == 20)
		return 1;
	if (d1 == 20 && d2 == -20)
		return 1;

	return 0;
}

void animation_draw()
{
	static int oldsec;
	static int do_second_animation = 0;
	static int do_minute_animation = 0;
	static int do_hour_animation = 0;
	static int do_align_animation = 0;
	static int do_triangle_animation = 0;
	static int do_sparkle_animation = 0;
	static int hour_dir;
	static unsigned hour_bright = 0;

	// update the LEDs once per second
	if (oldsec != RTCSEC
	&& !do_minute_animation
	&& !do_second_animation
	&& !do_triangle_animation
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
		if (triangle_time(RTCMIN, RTCSEC, hour_five))
		{
			do_triangle_animation = 120;
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

	if (do_triangle_animation)
	{
		triangle_animation(do_triangle_animation--);
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
	hour_bright += hour_dir;
	if (hour_bright == 16*8)
		hour_dir = -1;
	else
	if (hour_bright <= 8)
		hour_dir = +1;

	// set the default brightnesses for minute and second
	led_bright[0] = 16;
	led_bright[1] = (hour_bright / 8) + 1;
	led_bright[2] = 1;

	led_draw();
}
