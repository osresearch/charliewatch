/*! \file led.c
  \brief Charlieplexed LED driver
The leds array contains the positive and negative pins for each
of the LEDs, encoded into a single uint8_t.  The top four bits
define the port (0 == port 1, 1 == port 2) and the bottom three
bits define the pin on that port (0-7).

Pins P1.7 and P2.2 are unused and should be driven to ground.
Pins P2.6 and P2.7 are the RTC crystal.
Pin  P3.2 is the SW1 and should be pulled up (or driven to ground).
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"

#define PIN(port,pin)	(((port-1) << 3) | (pin << 0))
#define LED(x,y)	((x << 4) | (y << 0))

#define P1MASK	0x7F	// 01111111
#define P2MASK	0x3B	// 00111011

#define X1	PIN(2,5)
#define X2	PIN(1,6)
#define X3	PIN(1,0)
#define X4	PIN(1,1)
#define X5	PIN(1,2)
#define X6	PIN(1,4)
#define X7	PIN(1,3)
#define X8	PIN(1,5)
#define X9	PIN(2,0)
#define X10	PIN(2,1)
#define X11	PIN(2,3)
#define X12	PIN(2,4)

static const uint8_t leds[] = 
{
	// D1 - D60, two LEDs each
	// red color around the outside
	LED(X8, X6),
	LED(X10, X6),
	LED(X12, X6),
	LED(X2, X6),
	LED(X4, X6),
	LED(X7, X5),
	LED(X9, X5),
	LED(X11, X5),
	LED(X1, X5),
	LED(X3, X5),
	LED(X6, X4),
	LED(X8, X4),
	LED(X10, X4),
	LED(X12, X4),
	LED(X2, X4),
	LED(X5, X3),
	LED(X7, X3),
	LED(X9, X3),
	LED(X11, X3),
	LED(X1, X3),
	LED(X4, X2),
	LED(X6, X2),
	LED(X8, X2),
	LED(X10, X2),
	LED(X12, X2),
	LED(X4, X1),
	LED(X5, X1),
	LED(X7, X1),
	LED(X9, X1),
	LED(X11, X1),
	LED(X2, X12),
	LED(X4, X12),
	LED(X6, X12),
	LED(X8, X12),
	LED(X10, X12),
	LED(X1, X11),
	LED(X3, X11),
	LED(X5, X11),
	LED(X7, X11),
	LED(X9, X11),
	LED(X12, X10),
	LED(X2, X10),
	LED(X4, X10),
	LED(X6, X10),
	LED(X8, X10),
	LED(X11, X9),
	LED(X1, X9),
	LED(X3, X9),
	LED(X5, X9),
	LED(X7, X9),
	LED(X10, X8),
	LED(X12, X8),
	LED(X2, X8),
	LED(X4, X8),
	LED(X6, X8),
	LED(X9, X7),
	LED(X11, X7),
	LED(X1, X7),
	LED(X3, X7),
	LED(X5, X7),

	// orange color around the outside
	LED(X7, X6),
	LED(X9, X6),
	LED(X11, X6),
	LED(X1, X6),
	LED(X3, X6),
	LED(X6, X5),
	LED(X8, X5),
	LED(X10, X5),
	LED(X12, X5),
	LED(X2, X5),
	LED(X5, X4),
	LED(X7, X4),
	LED(X9, X4),
	LED(X11, X4),
	LED(X1, X4),
	LED(X4, X3),
	LED(X6, X3),
	LED(X8, X3),
	LED(X10, X3),
	LED(X12, X3),
	LED(X3, X2),
	LED(X5, X2),
	LED(X7, X2),
	LED(X9, X2),
	LED(X11, X2),
	LED(X2, X1),
	LED(X3, X1),
	LED(X6, X1),
	LED(X8, X1),
	LED(X10, X1),
	LED(X1, X12),
	LED(X3, X12),
	LED(X5, X12),
	LED(X7, X12),
	LED(X9, X12),
	LED(X12, X11),
	LED(X2, X11),
	LED(X4, X11),
	LED(X6, X11),
	LED(X8, X11),
	LED(X11, X10),
	LED(X1, X10),
	LED(X3, X10),
	LED(X5, X10),
	LED(X7, X10),
	LED(X10, X9),
	LED(X12, X9),
	LED(X2, X9),
	LED(X4, X9),
	LED(X6, X9),
	LED(X9, X8),
	LED(X11, X8),
	LED(X1, X8),
	LED(X3, X8),
	LED(X5, X8),
	LED(X8, X7),
	LED(X10, X7),
	LED(X12, X7),
	LED(X2, X7),
	LED(X4, X7),

	// hour LEDs (0805 sized)
	LED(X6, X7),
	LED(X5, X6),
	LED(X4, X5),
	LED(X3, X4),
	LED(X2, X3),
	LED(X1, X2),
	LED(X12, X1),
	LED(X11, X12),
	LED(X10, X11),
	LED(X9, X10),
	LED(X8, X9),
	LED(X7, X8),
};

#define NUM_LEDS (sizeof(leds)/sizeof(*leds))

void led_on(uint8_t i)
{
	// tri-state all LED lines, not touching any of the others
	P1DIR &= ~P1MASK;
	P1OUT &= ~P1MASK;
	P2DIR &= ~P2MASK;
	P2OUT &= ~P2MASK;

	const uint8_t led = leds[i];
	const uint8_t y = (led >> 4) & 0x7;
	const uint8_t py = led & 0x80;
	const uint8_t x = (led >> 0) & 0x7;
	const uint8_t px = led & 0x08;

	// drive the X pin to ground
	if (px)
		P2DIR |= 1 << x;
	else
		P1DIR |= 1 << x;

	// drive the Y pin high
	if (py)
	{
		P2DIR |= 1 << y;
		P2OUT |= 1 << y;
	} else {
		P1DIR |= 1 << y;
		P1OUT |= 1 << y;
	}
}

void led_off(void)
{
	// set all the LED lines to output, drive to ground
	P1OUT &= ~P1MASK;
	P2OUT &= ~P2MASK;
	P1DIR |= P1MASK;
	P2DIR |= P2MASK;
}


void led_dither(uint8_t i, uint8_t bright)
{
#if 1
	// come back to fix this later
	led_on(i);
	led_off();
#else
	const uint8_t x = (leds[i] >> 4) & 0xF;
	const uint8_t y = (leds[i] >> 0) & 0xF;

	// tri-state all LED lines, leaving the others driven
	P3DIR = ~0x1F;
	P2DIR = ~0x0F;

	// 0-4 are on P3.0 - P3.4
	// 5-8 are on P2.0 - P2.3
	if (x < 5)
		P3DIR |= 1 << (x - 0);
	else
		P2DIR |= 1 << (x - 5);

	if (y < 5) {
		const uint8_t mask = 1 << (y - 0);
		P3DIR |= mask;

		do {
			P3OUT = mask;
			P3OUT = 0;
		} while(bright--);
	} else {
		const uint8_t mask = 1 << (y - 5);
		P2DIR |= mask;

		do {
			P2OUT = mask;
			P2OUT = 0;
		} while(bright--);
	}

	led_off();
#endif
}

void led_test(void)
{
	int wait = 5000;
	int i;

	for(i = 0 ; i < 120 ; i++)
	{
		led_on(i);
		delay((i == 0 || i == 60) ? 5*wait : wait);
		led_off();
	}

	for(i = 120 ; i < NUM_LEDS ; i++)
	{
		led_on(i);
		delay(wait*5);
		led_off();
	}
}
