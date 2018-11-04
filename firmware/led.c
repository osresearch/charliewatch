/*! \file led.c

  \brief Charlieplexed LED driver
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "led.h"

#define LED(x,y)	((x << 4) | (y << 0))

static const uint8_t leds[] = 
{
	//  0 - 59, the minutes
	LED(2,0), // led01,  00
	LED(3,0), // led02,  01
	LED(4,0), // led03,  02
	LED(5,0), // led04,  03
	LED(6,0), // led05,  04
	LED(7,0), // led06,  05
	LED(8,0), // led07,  06

	LED(8,1), // led15,  07
	LED(7,1), // led14,  08
	LED(6,1), // led13,  09
	LED(5,1), // led12,  10
	LED(4,1), // led11,  11
	LED(3,1), // led10,  12
	LED(2,1), // led09,  13
	LED(0,1), // led08,  14

	LED(0,2), // led16,  15
	LED(1,2), // led17,  16
	LED(3,2), // led18,  17
	LED(4,2), // led19,  18
	LED(5,2), // led20,  19
	LED(6,2), // led21,  20
	LED(7,2), // led22,  21
	LED(8,2), // led23,  22

	LED(8,3), // led31,  23
	LED(7,3), // led30,  24
	LED(6,3), // led29,  25
	LED(5,3), // led28,  26
	LED(4,3), // led27,  27
	LED(2,3), // led26,  28
	LED(1,3), // led25,  29
	LED(0,3), // led24,  30

	LED(0,4), // led32,  31
	LED(1,4), // led33,  32
	LED(2,4), // led34,  33
	LED(3,4), // led35,  34
	LED(5,4), // led36,  35
	LED(6,4), // led37,  36
	LED(7,4), // led38,  37
	LED(8,4), // led39,  38

	LED(8,5), // led47,  39
	LED(7,5), // led46,  40
	LED(6,5), // led45,  41
	LED(4,5), // led44,  42
	LED(3,5), // led43,  43
	LED(2,5), // led42,  44
	LED(1,5), // led41,  45
	LED(0,5), // led40,  46

	LED(0,6), // led48,  47
	LED(1,6), // led49,  48
	LED(2,6), // led50,  49
	LED(3,6), // led51,  50
	LED(4,6), // led52,  51
	LED(5,6), // led53,  52
	LED(7,6), // led54,  53
	LED(8,6), // led55,  54

	LED(8,7), // led63,  55
	LED(6,7), // led62,  56
	LED(5,7), // led61,  57
	LED(4,7), // led60,  58
	LED(1,0), // led0,   59

	// 60 - 71, the hours
	LED(1,7), // led57,  0
	LED(3,7), // led59,  1
	LED(2,7), // led58,  2
	LED(0,8), // led64,  3
	LED(2,8), // led66,  4
	LED(6,8), // led70,  5
	LED(0,7), // led56,  6
	LED(5,8), // led69,  7
	LED(7,8), // led71,  8
	LED(1,8), // led65,  9
	LED(3,8), // led67,  10
	LED(4,8), // led68,  11
};

#define NUM_LEDS (sizeof(leds)/sizeof(*leds))

void led_on(uint8_t i)
{
	// tri-state all LED lines, leaving the others driven
	P3DIR = ~0x1F;
	P3OUT = 0;
	P2DIR = ~0x0F;
	P2OUT = 0;

	const uint8_t x = (leds[i] >> 4) & 0xF;
	const uint8_t y = (leds[i] >> 0) & 0xF;

	// 0-4 are on P3.0 - P3.4
	// 5-8 are on P2.0 - P2.3
	if (x < 5)
		P3DIR |= 1 << (x - 0);
	else
		P2DIR |= 1 << (x - 5);

	if (y < 5) {
		P3DIR |= 1 << (y - 0);
		P3OUT |= 1 << (y - 0);
	} else {
		P2DIR |= 1 << (y - 5);
		P2OUT |= 1 << (y - 5);
	}
}

void led_off(void)
{
	// set all the LED lines to output, drive to ground
	P2OUT = 0;
	P3OUT = 0;
	P3DIR = 0xFF;
	P2DIR = 0xFF;
}


void led_dither(uint8_t i, uint8_t bright)
{
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
}

void led_test(void)
{
	int i, j;
	for(i = 0 ; i < 72 ; i++)
	{
		for(j = 0 ; j < 20 ; j++)
		{
			led_on(i); // delay(4);
			led_off();
			delay(200);
		}
	}
}
