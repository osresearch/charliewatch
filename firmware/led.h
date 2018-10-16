#pragma once

// "Framebuffer" for the four LEDs that are on,
// with configurable brightness.
#define NUM_DISPLAY 3
extern uint8_t led_display[NUM_DISPLAY];
extern uint8_t led_bright[NUM_DISPLAY];

// Draw the "framebuffer"
void led_draw();

void animation_draw();

// Raw method to turn on an LED
void led_on(uint8_t i);

// Turn off all LEDs
void led_off();

// Spin loop
void delay(unsigned len);
