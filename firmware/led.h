#pragma once

// "Framebuffer" for the four LEDs that are on,
// with decreasing brightness.
#define NUM_LEDS_ON 4
extern uint8_t led_display[NUM_LEDS_ON];

// Draw the "framebuffer"
void led_draw();

// Raw method to turn on an LED
void led_on(uint8_t i);

// Turn off all LEDs
void led_off();

// Spin loop
void delay(unsigned len);
