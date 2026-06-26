/*
 * Medication Wristband
 *
 * NFC-based medication administration verification for caretakers.
 * Scans a patient's NFC tag, checks the schedule against the DS3231 RTC,
 * shows a color-coded status, and logs confirmed doses to an SD card.
 *
 * Hardware:
 *   MFRC522 NFC reader  -- SPI (MOSI=11, MISO=12, SCK=13, SS=10, RST=9)
 *   DS3231 RTC          -- I2C (SDA=A4, SCL=A5)
 *   SD card module      -- SPI (CS=4, shares MOSI/MISO/SCK with NFC reader)
 *   RGB LED (common cathode) -- pins 5 (R), 6 (G), 3 (B)
 *   Buzzer              -- pin 7
 *   Confirm button      -- pin 8 (INPUT_PULLUP, active LOW)
 *
 * Libraries required (install via Library Manager):
 *   MFRC522   by GithubCommunity
 *   RTClib    by Adafruit
 *   SD        (built-in with Arduino IDE)
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#include "config.h"
#include "led.h"
#include "nfc.h"
#include "rtc.h"
#include "medication.h"
#include "logger.h"

MFRC522 nfcReader(SS_PIN, RST_PIN);

// State machine states
enum State { IDLE, AWAITING_CONFIRM };

static State    state         = IDLE;
static int      pendingMedIdx = -1;
static LedColor pendingColor  = LED_OFF;

// Button debounce state
#define DEBOUNCE_MS 50
static bool          lastButtonRaw  = HIGH;
static bool          buttonState    = HIGH;
static unsigned long lastDebounceMs = 0;

// Returns true on the falling edge of the button after debounce.
static bool buttonJustPressed() {
  bool raw = digitalRead(BUTTON_PIN);
  if (raw != lastButtonRaw) {
    lastDebounceMs = millis();
    lastButtonRaw  = raw;
  }
  if ((millis() - lastDebounceMs) > DEBOUNCE_MS) {
    if (raw != buttonState) {
      buttonState = raw;
      if (buttonState == LOW) return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();
  nfcReader.PCD_Init();

  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_BLUE,   OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  setLED(LED_OFF);

  // Both peripherals must initialize successfully for the device to operate
  bool rtcReady    = rtcBegin();
  bool loggerReady = loggerBegin();

  if (!rtcReady || !loggerReady) {
    Serial.println(F("Startup error. Check hardware and retry."));
    // Blink red indefinitely to signal a hardware fault
    while (true) {
      alert(LED_RED_C, 1);
      delay(1000);
    }
  }

  Serial.println(F("Medication wristband ready."));
}

void loop() {
  bool pressed = buttonJustPressed();

  switch (state) {

    case IDLE: {
      String uid = readUID(nfcReader);
      if (uid.length() == 0) break;

      Serial.print(F("Scanned UID: "));
      Serial.println(uid);

      int idx = findMedication(uid);
      if (idx == -1) {
        Serial.println(F("Tag not recognized."));
        alert(LED_RED_C);
        nfcReader.PICC_HaltA();
        break;
      }

      MedStatus status = checkMedication(idx);

      switch (status) {
        case MED_OK:    pendingColor = LED_GREEN_C;  break;
        case MED_EARLY: pendingColor = LED_ORANGE_C; break;
        case MED_LATE:  pendingColor = LED_RED_C;    break;
      }

      alert(pendingColor);

      if (status == MED_EARLY) {
        // Too early: inform and return to idle without waiting for confirmation
        Serial.print(F("Too early for: "));
        Serial.println(medications[idx].medName);
        nfcReader.PICC_HaltA();
        break;
      }

      // On-time or late: hold the color and wait for the caretaker to confirm
      pendingMedIdx = idx;
      setLED(pendingColor);
      Serial.print(F("Press button to confirm: "));
      Serial.println(medications[idx].medName);
      state = AWAITING_CONFIRM;
      break;
    }

    case AWAITING_CONFIRM: {
      if (pressed) {
        char timestamp[17];
        rtcGetTimestamp(timestamp);
        logMedication(pendingMedIdx, "Standard Dose", timestamp);

        Serial.print(F("Logged at "));
        Serial.println(timestamp);

        setLED(LED_OFF);
        nfcReader.PICC_HaltA();
        pendingMedIdx = -1;
        state = IDLE;
      }
      break;
    }
  }
}
