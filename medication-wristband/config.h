#pragma once
#include "types.h"

// NFC reader SPI pins
#define SS_PIN   10
#define RST_PIN   9

// SD card chip select
#define SD_CS_PIN 4

// RGB LED pins (common cathode, must be PWM-capable)
#define LED_RED   5
#define LED_GREEN 6
#define LED_BLUE  3

// Buzzer pin
#define BUZZER_PIN 7

// Confirm button pin (uses INPUT_PULLUP; LOW = pressed)
#define BUTTON_PIN 8

// Tolerance window in minutes for scheduled (non-PRN) medications
#define EARLY_WINDOW_MIN 60
#define LATE_WINDOW_MIN  60

// SD card file names
#define META_FILE "META.BIN" // stores LogHeader (4 bytes)
#define LOG_FILE  "LOG.BIN"  // stores LogRecord array (MAX_SD_LOGS * 64 bytes)

// NFC UIDs — replace with UIDs printed to Serial when you scan your actual tags
const char* nfcUIDs[MAX_MEDICATIONS] = {
  "AB 12 CD 34",
  "EF 56 GH 78",
  "IJ 90 KL 12",
  "MN 34 OP 56",
  "QR 78 ST 90"
};

// Medication schedule — edit this to match your patients and drugs
// { patientName, medName, hour, minute, prn, cooldownMin, lastGiven }
Medication medications[MAX_MEDICATIONS] = {
  { "John Doe", "Aspirin",         8,  0, false,   0, 0 },
  { "John Doe", "Ibuprofen",      12,  0, true,  240, 0 }, // PRN every 4 h
  { "Jane Doe", "Metformin",       9,  0, false,   0, 0 },
  { "Jane Doe", "Amoxicillin",    15, 30, false,   0, 0 },
  { "John Doe", "PRN Painkiller",  0,  0, true,  180, 0 }  // PRN every 3 h
};
