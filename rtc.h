#pragma once

// Initialize the DS3231 over I2C. Returns true on success.
// If the RTC lost power, its time is reset to the firmware compile time as a fallback.
bool rtcBegin();

// Fill buf with the current datetime formatted as "YYYY-MM-DD HH:MM\0".
// buf must be at least 17 bytes.
void rtcGetTimestamp(char* buf);

// Returns the current time as minutes since midnight (0-1439).
// Falls back to millis()-based estimate if the RTC is unavailable.
int rtcGetMinutes();
