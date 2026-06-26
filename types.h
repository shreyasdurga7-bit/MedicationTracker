#pragma once
#include <stdint.h>

#define MAX_MEDICATIONS   5
#define MAX_SD_LOGS     200

// Medication definition stored in firmware
struct Medication {
  const char*   patientName;
  const char*   medName;
  int           hour;        // Scheduled hour (0-23); ignored when prn is true
  int           minute;      // Scheduled minute; ignored when prn is true
  bool          prn;         // PRN (as-needed) flag
  int           cooldownMin; // Minimum minutes between PRN doses
  unsigned long lastGiven;   // millis() timestamp of last confirmed dose
};

// Fixed-size binary record written to SD card (64 bytes per entry)
struct LogRecord {
  char    patientName[20];
  char    medName[20];
  char    timestamp[17]; // "YYYY-MM-DD HH:MM\0"
  char    dose[6];
  uint8_t _pad[1];       // padding to reach exactly 64 bytes
};

// Header stored in META.BIN (4 bytes total)
struct LogHeader {
  uint16_t writeIndex; // Next slot to write into (0 to MAX_SD_LOGS-1, wraps)
  uint16_t count;      // Total entries written, capped at MAX_SD_LOGS
};
