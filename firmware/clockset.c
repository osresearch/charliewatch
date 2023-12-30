/*! \file Clock setting functions.

  \brief Allow the user to set the clock via button presses.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"
#include "button.h"
#include "rtc.h"

static unsigned temp_hour;
static unsigned temp_min;
static unsigned temp_sec;
static unsigned timeout;
static unsigned selected;
extern unsigned watch_mode;

// called every 16ms by the RTC interrupt
void clockset_draw(void)
{
	if (button_long)
	{
		// just entered this mode; capture the hour/min/sec
		temp_hour = RTCHOUR;
		temp_min = RTCMIN;
		temp_sec = RTCSEC;
		timeout = 0;

		// toggle which setting (minute or hour) we have selected
		selected = (selected + 1) % 2;
	}

	if (button_short)
	{
		timeout = 0;

		if (selected == 0)
			temp_min = (temp_min + 1) % 60;
		else
			temp_hour = (temp_hour + 1) % 24;

		RTCHOUR = temp_hour;
		RTCMIN = temp_min;
		RTCSEC = temp_sec;
		//SetRTCHOUR(temp_hour);
		//SetRTCMIN(temp_min);
		//SetRTCSEC(temp_sec);
	}

	// no button presses in 15 seconds => go back to clock mode
	if (++timeout > 15*60)
	{
		watch_mode = 0;
		return;
	}

	// the second always stays on
	led_on(temp_sec);
	led_off();

	if (selected == 0)
	{
		// hour always on
		led_on((temp_hour % 12) + 60);
		delay(4);
		led_off();

		// flash the minute on and off
		if ((timeout % 32) < 24)
		{
			led_on(temp_min);
			delay(4);
		}
	} else {
		// minute always on
		led_on(temp_min);
		delay(4);

		// flash the hour on and off
		if ((timeout % 32) < 24)
		{
			led_on((temp_hour % 12) + 60);
			delay(4);
		}
	}
}
