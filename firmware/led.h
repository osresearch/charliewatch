#pragma once

// "Framebuffer" for the four LEDs that are on,
// with configurable brightness.
#define NUM_DISPLAY 3
extern uint8_t led_display[NUM_DISPLAY];
extern uint8_t led_bright[NUM_DISPLAY];

// Draw the "framebuffer"
void led_draw(void);

void animation_draw(void);

// Raw method to turn on an LED
void led_on(uint8_t i);
void led_dither(uint8_t i, uint8_t bright);

// Turn off all LEDs
void led_off(void);

// Spin loop
void delay(unsigned len);

// test all the LEDs
void led_test(void);
