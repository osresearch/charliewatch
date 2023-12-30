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
#include "adc.h"
#include "power.h"
#include "button.h"

// msp430g2553 has no RTC
uint8_t RTCHOUR;
uint8_t RTCMIN;
uint8_t RTCSEC;

//! Activates and configures the module, but does not turn on reference.
void ref_init(){
/*
  //Setting the master bit disables legacy mode.
  REFCTL0|=REFMSTR;

  //Wait for register to not be busy before modifying voltage.
  while(REFCTL0 & REFGENBUSY);
  
  //2.5V reference.
  REFCTL0 &= ~BGMODE;
  REFCTL0|=REFVSEL_2;
*/
}


//! Main method.
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	ref_init();
	//uart_init();

	// drive all the ports to ground to avoid power drain
	P3DIR = 0xFF;
	P3OUT = 0;
	P2DIR = 0xFF;
	P2OUT = 0;
	P1DIR = 0xFF;
	P1OUT = 0;

	// xxx: configure this
	//button_init();

	// turn off all the LEDs to reduce power
	led_off();

#if 0
	led_test();
#else
	while(1) {
		led_test();
		delay(1000);
	}
#endif

	//rtc_init();
	//ucs_init(); // doesn't work if crystal isn't there?

	// Setup and enable WDT 16ms, ACLK, interval timer
	WDTCTL = WDT_ADLY_16;
	//SFRIE1 |= WDTIE;

	// go into low power mode
	//power_setvcore(0);
	//ucs_slow();

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

static void voltage_draw(void)
{
	unsigned i;
	unsigned raw_volts = 512;
#if 0
	unsigned raw_volts = adc12_single_conversion(
		REFVSEL_1, ADC12SHT0_10, ADC12INCH_11);


	static unsigned bright = 0;
	bright = (bright + 1) % 128;

	for(i = 0 ; i < 12 ; i++)
	{
		if (0 == (raw_volts & (1<<i)))
			continue;
		led_dither(60+i, bright > 64 ? 128 - bright : bright);
	}

	unsigned scaled = raw_volts / 68;
	for(i = 0 ; i < scaled ; i++)
	{
		led_on(i);
	}
#else
	unsigned millivolts = (1250 * (uint32_t) raw_volts) / 1024;
	led_on(60 + (millivolts / 1000));

	unsigned mv = ((millivolts % 1000) * 60) / 1000;
	for(i = 0 ; i < mv ; i++)
	{
		led_on(i);
	}
#endif
		
	led_off();
}

static void (*const modes[])(void) = {
	//voltage_draw,
	animation_draw,
	stopwatch_draw,
	clockset_draw,
	clockset_draw,
	voltage_draw,
};

static const unsigned mode_count = sizeof(modes) / sizeof(*modes);
unsigned watch_mode = 0;


//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR)))
watchdog_timer(void)
{
	//ucs_fast();

	button_update();

	// long hold advances mode
	if (button_long)
		watch_mode = (watch_mode + 1) % mode_count;

	modes[watch_mode]();

	led_off();
	//ucs_slow();
}
