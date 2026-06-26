#include <Arduino.h>
#include <RTClib.h>
#include "rtc.h"

static RTC_DS3231 rtc;
static bool rtcOk = false;

bool rtcBegin() {
  rtcOk = rtc.begin();
  if (!rtcOk) {
    Serial.println(F("RTC not found. Check I2C wiring on A4/A5."));
    return false;
  }
  if (rtc.lostPower()) {
    // Compile-time fallback. Set your RTC properly before deploying.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(F("RTC lost power — time reset to compile time. Set it manually."));
  }
  return true;
}

void rtcGetTimestamp(char* buf) {
  if (!rtcOk) {
    strncpy(buf, "0000-00-00 00:00", 17);
    return;
  }
  DateTime now = rtc.now();
  snprintf(buf, 17, "%04d-%02d-%02d %02d:%02d",
    now.year(), now.month(), now.day(),
    now.hour(), now.minute());
}

int rtcGetMinutes() {
  if (!rtcOk) {
    // millis() fallback: wraps every 24 h and resets on power-cycle
    return (int)((millis() / 60000UL) % 1440);
  }
  DateTime now = rtc.now();
  return now.hour() * 60 + now.minute();
}
