#pragma once

// Initialize the SD card and prepare the log files.
// Must be called in setup() before any logging. Returns true on success.
// On success the SD contains META.BIN (4-byte header) and LOG.BIN (pre-allocated record array).
bool loggerBegin();

// Write a medication administration event to the circular buffer on the SD card,
// then dump the full log as CSV to Serial. The timestamp string must be 16 chars
// in "YYYY-MM-DD HH:MM" format (use rtcGetTimestamp).
void logMedication(int idx, const char* dose, const char* timestamp);

// Print all stored log entries to Serial in CSV format, oldest entry first.
void exportCSV();
