#pragma once

// Color constants used for setLED and alert
enum LedColor {
  LED_OFF      = -1,
  LED_GREEN_C  =  0,
  LED_ORANGE_C =  1,
  LED_RED_C    =  2
};

// Set the RGB LED to a single color (or off)
void setLED(LedColor color);

// Flash the LED and beep the buzzer 'times' times
void alert(LedColor color, int times = 2);
