#include <Arduino.h>
#include "medication.h"
#include "config.h"
#include "rtc.h"

MedStatus checkMedication(int idx) {
  Medication& med = medications[idx];

  if (med.prn) {
    // lastGiven == 0 means the medication has never been given, so always allow
    if (med.lastGiven != 0) {
      unsigned long elapsedMin = (millis() - med.lastGiven) / 60000UL;
      if (elapsedMin < (unsigned long)med.cooldownMin) return MED_EARLY;
    }
    return MED_OK;
  }

  int now       = rtcGetMinutes();
  int scheduled = med.hour * 60 + med.minute;

  if (now < scheduled - EARLY_WINDOW_MIN) return MED_EARLY;
  if (now > scheduled + LATE_WINDOW_MIN)  return MED_LATE;
  return MED_OK;
}
