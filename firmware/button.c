#include <msp430.h>
#include <stdint.h>
#include "button.h"

unsigned button_short;
unsigned button_long;

void button_init(void)
{
	// configure button as input with internal pullup
	// (v0.1 is reusing the RX pin)
	P1SEL &= ~(1 << 5);
	P1DIR &= ~(1 << 5);
	P1REN |=  (1 << 5);
	P1OUT |=  (1 << 5);
}


// check the button status
void button_update(void)
{
	static unsigned button_counter;
	button_long = button_short = 0;

	// if button is pressed, increment hold counter
	if ((P1IN & (1 << 5)) == 0)
	{
		button_counter++;

		// check for long hold (~2 seconds)
		// only have it true for one cycle
		if ((button_counter % 128) == 0)
		{
			button_long = 1;
		}

		return;
	}

	// button has been released
	// if held and released for the right amount of time
	if (5 < button_counter && button_counter < 60)
		button_short = 1;

	button_counter = 0;
}
