#include <Arduino.h>
#include <SD.h>
#include "logger.h"
#include "config.h"

static bool sdOk = false;

// Byte offset of a given slot in LOG.BIN
static uint32_t slotOffset(uint16_t slot) {
  return (uint32_t)slot * sizeof(LogRecord);
}

// Read the circular buffer header from META.BIN into h.
// Returns false if the file cannot be opened.
static bool readHeader(LogHeader& h) {
  File f = SD.open(META_FILE, FILE_READ);
  if (!f) return false;
  f.read((uint8_t*)&h, sizeof(h));
  f.close();
  return true;
}

// Overwrite META.BIN with updated header values.
// The file is removed and rewritten because Arduino's SD library does not
// support in-place random writes to existing files reliably on all hardware.
static bool writeHeader(const LogHeader& h) {
  SD.remove(META_FILE);
  File f = SD.open(META_FILE, FILE_WRITE);
  if (!f) return false;
  f.write((const uint8_t*)&h, sizeof(h));
  f.close();
  return true;
}

bool loggerBegin() {
  sdOk = SD.begin(SD_CS_PIN);
  if (!sdOk) {
    Serial.println(F("SD init failed. Check wiring and card format (FAT32)."));
    return false;
  }

  // Create META.BIN with a zeroed header if it does not exist
  if (!SD.exists(META_FILE)) {
    LogHeader h = {0, 0};
    if (!writeHeader(h)) {
      Serial.println(F("Failed to create META.BIN."));
      sdOk = false;
      return false;
    }
    Serial.println(F("META.BIN created."));
  }

  // Pre-allocate LOG.BIN with empty records so we can seek to any slot later.
  // This writes MAX_SD_LOGS * 64 = 12,800 bytes once and is skipped on subsequent boots.
  if (!SD.exists(LOG_FILE)) {
    File f = SD.open(LOG_FILE, FILE_WRITE);
    if (!f) {
      Serial.println(F("Failed to create LOG.BIN."));
      sdOk = false;
      return false;
    }
    LogRecord empty;
    memset(&empty, 0, sizeof(empty));
    for (uint16_t i = 0; i < MAX_SD_LOGS; i++) {
      f.write((const uint8_t*)&empty, sizeof(empty));
    }
    f.close();
    Serial.println(F("LOG.BIN pre-allocated."));
  }

  Serial.println(F("SD logger ready."));
  return true;
}

void logMedication(int idx, const char* dose, const char* timestamp) {
  if (!sdOk) {
    Serial.println(F("SD unavailable. Entry not saved."));
    return;
  }

  LogHeader h;
  if (!readHeader(h)) {
    Serial.println(F("Cannot read META.BIN."));
    return;
  }

  // Build the fixed-size record
  LogRecord rec;
  memset(&rec, 0, sizeof(rec));
  strncpy(rec.patientName, medications[idx].patientName, sizeof(rec.patientName) - 1);
  strncpy(rec.medName,     medications[idx].medName,     sizeof(rec.medName)     - 1);
  strncpy(rec.timestamp,   timestamp,                    sizeof(rec.timestamp)   - 1);
  strncpy(rec.dose,        dose,                         sizeof(rec.dose)        - 1);

  // Write the record into the correct slot in LOG.BIN
  File logFile = SD.open(LOG_FILE, FILE_WRITE);
  if (!logFile) {
    Serial.println(F("Cannot open LOG.BIN for writing."));
    return;
  }
  logFile.seek(slotOffset(h.writeIndex));
  logFile.write((const uint8_t*)&rec, sizeof(rec));
  logFile.close();

  // Advance the circular buffer pointer and update the count
  h.writeIndex = (h.writeIndex + 1) % MAX_SD_LOGS;
  if (h.count < MAX_SD_LOGS) h.count++;

  if (!writeHeader(h)) {
    Serial.println(F("Warning: failed to update META.BIN."));
  }

  // Update the in-RAM lastGiven so PRN cooldown works within the same session
  medications[idx].lastGiven = millis();

  exportCSV();
}

void exportCSV() {
  if (!sdOk) {
    Serial.println(F("SD unavailable."));
    return;
  }

  LogHeader h;
  if (!readHeader(h) || h.count == 0) {
    Serial.println(F("No log entries found."));
    return;
  }

  File logFile = SD.open(LOG_FILE, FILE_READ);
  if (!logFile) {
    Serial.println(F("Cannot open LOG.BIN for reading."));
    return;
  }

  // When the buffer is full, the oldest entry is at writeIndex (it was just overwritten).
  // When not yet full, entries start at slot 0.
  uint16_t startSlot = (h.count < MAX_SD_LOGS) ? 0 : h.writeIndex;

  Serial.println(F("Patient,Medication,Time Given,Dose"));
  for (uint16_t i = 0; i < h.count; i++) {
    uint16_t slot = (startSlot + i) % MAX_SD_LOGS;
    logFile.seek(slotOffset(slot));
    LogRecord rec;
    logFile.read((uint8_t*)&rec, sizeof(rec));
    Serial.print(rec.patientName); Serial.print(F(","));
    Serial.print(rec.medName);     Serial.print(F(","));
    Serial.print(rec.timestamp);   Serial.print(F(","));
    Serial.println(rec.dose);
  }

  logFile.close();
  Serial.println(F("End of log"));
}
