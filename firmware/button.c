#include <msp430.h>
#include <stdint.h>
#include "button.h"

unsigned button_short;
unsigned button_long;

void button_init(void)
{
	// configure button as input with internal pullup
	// (v0.1 is reusing the RX pin, 1.5)
//#define BUTTON 5
	// (v0.2 has a separate button on 1.7)
#define BUTTON 7
	P1SEL &= ~(1 << BUTTON);
	P1DIR &= ~(1 << BUTTON);
	P1REN |=  (1 << BUTTON);
	P1OUT |=  (1 << BUTTON);
}


// check the button status
void button_update(void)
{
	static unsigned button_counter;
	button_long = button_short = 0;

	// if button is pressed, increment hold counter
	if ((P1IN & (1 << BUTTON)) == 0)
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
	if (3 < button_counter && button_counter < 60)
		button_short = 1;

	button_counter = 0;
}
