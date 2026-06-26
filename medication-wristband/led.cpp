#include <Arduino.h>
#include "led.h"
#include "config.h"

void setLED(LedColor color) {
  switch (color) {
    case LED_GREEN_C:
      analogWrite(LED_RED,   0);
      analogWrite(LED_GREEN, 255);
      analogWrite(LED_BLUE,  0);
      break;
    case LED_ORANGE_C:
      analogWrite(LED_RED,   255);
      analogWrite(LED_GREEN, 165);
      analogWrite(LED_BLUE,  0);
      break;
    case LED_RED_C:
      analogWrite(LED_RED,   255);
      analogWrite(LED_GREEN, 0);
      analogWrite(LED_BLUE,  0);
      break;
    default: // LED_OFF
      analogWrite(LED_RED,   0);
      analogWrite(LED_GREEN, 0);
      analogWrite(LED_BLUE,  0);
      break;
  }
}

void alert(LedColor color, int times) {
  for (int i = 0; i < times; i++) {
    setLED(color);
    tone(BUZZER_PIN, 1000, 300);
    delay(300);
    setLED(LED_OFF);
    delay(300);
  }
}
