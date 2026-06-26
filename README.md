Medication Wristband
NFC-based medication administration verification for caretakers. Scan a patient's NFC wristband tag to identify the medication, check the schedule against a real-time clock, get a visual and audio status, confirm with a button press, and log the event to an SD card.

Hardware
The MFRC522 NFC reader connects via SPI on pins SS=10, RST=9, MOSI=11, MISO=12, SCK=13. The DS3231 RTC connects via I2C on SDA=A4 and SCL=A5. The SD card module also uses SPI with CS on pin 4, sharing MOSI, MISO, and SCK with the NFC reader. The RGB LED uses a common cathode wired to GND with R on pin 5, G on pin 6, and B on pin 3 (all PWM). The buzzer goes on pin 7 and the confirm button on pin 8 wired to GND using INPUT_PULLUP.

Libraries
Install these via the Arduino Library Manager before compiling: MFRC522 by GithubCommunity, RTClib by Adafruit, and SD which is included with the Arduino IDE.

LED Status
Green means on time and safe to administer. Orange means too early — either the cooldown hasn't elapsed or the scheduled window hasn't opened yet. Red means overdue or the tag is unrecognized. If the RTC or SD card fails at startup, the LED blinks red continuously.

Setup
Wire the hardware as described above. Insert a FAT32-formatted SD card. Flash the sketch — on first boot META.BIN and LOG.BIN are created automatically. Open Serial Monitor at 9600 baud. Scan each NFC tag and note the UID printed to Serial, then update nfcUIDs[] and medications[] in config.h to match. The RTC sets itself to compile time on first run or after a power loss, so set the correct time manually after that.

SD Card Files
META.BIN is a 4-byte file storing the write index and entry count. LOG.BIN is a 12,800-byte pre-allocated array of 200 fixed-size 64-byte log records. The log is a circular buffer — once 200 entries are written, the oldest is overwritten. You can pull the SD card at any time to read LOG.BIN directly, or trigger exportCSV() via Serial to print all entries in chronological order.

CSV Format
The exported CSV has four columns: Patient, Medication, Time Given, and Dose. An example row looks like: John Doe, Aspirin, 2025-01-15 08:03, Standard Dose.

Notes
The DS3231 retains time through power cycles using its onboard coin cell. Replace the cell if the RTC resets unexpectedly. The lastGiven value for PRN medications is stored in RAM only and resets on reboot, but the SD log remains intact. To extend the history beyond 200 entries, increase MAX_SD_LOGS in types.h — each extra entry costs 64 bytes of SD space.




