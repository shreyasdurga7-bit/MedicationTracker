# Medication Wristband

NFC-based medication administration verification for caretakers. Scan a patient's NFC wristband tag to identify the medication, check the schedule against a real-time clock, get a visual and audio status, confirm with a button press, and log the event to an SD card.

## Hardware

| Component | Connection |
|-----------|------------|
| MFRC522 NFC reader | SPI — SS=10, RST=9, MOSI=11, MISO=12, SCK=13 |
| DS3231 RTC | I2C — SDA=A4, SCL=A5 |
| SD card module | SPI — CS=4, shares MOSI/MISO/SCK with NFC reader |
| RGB LED (common cathode) | R=5 (PWM), G=6 (PWM), B=3 (PWM), common→GND |
| Buzzer | Pin 7 |
| Confirm button | Pin 8 → GND (INPUT_PULLUP) |

## Libraries

Install these via the Arduino Library Manager before compiling:

- **MFRC522** by GithubCommunity
- **RTClib** by Adafruit
- **SD** (included with the Arduino IDE)

## LED Status

| Color | Meaning |
|-------|---------|
| Green | On time — safe to administer |
| Orange | Too early — cooldown not elapsed or outside the scheduled window |
| Red | Overdue, or unrecognized tag |

On a hardware fault at startup (RTC or SD unavailable) the LED blinks red continuously.

## Setup

1. Wire the hardware as shown in the table above.
2. Insert a FAT32-formatted SD card.
3. Flash the sketch. On first boot, `META.BIN` and `LOG.BIN` are created automatically.
4. Open Serial Monitor at **9600 baud**.
5. Scan each NFC tag — the UID is printed to Serial. Update `nfcUIDs[]` and `medications[]` in `config.h` to match.
6. Set the RTC time by uploading the sketch (it self-sets to compile time on first run or after a power loss).

## SD Card Files

| File | Contents |
|------|----------|
| `META.BIN` | 4-byte header: `writeIndex` and `count` |
| `LOG.BIN` | 12,800-byte pre-allocated array of 200 fixed-size (64-byte) log records |

The log is a circular buffer. Once 200 entries are written, the oldest is overwritten. Pull the SD card at any time and read `LOG.BIN` — or trigger `exportCSV()` via Serial to print all entries in chronological order.

## CSV Format

```
Patient,Medication,Time Given,Dose
John Doe,Aspirin,2025-01-15 08:03,Standard Dose
```

## Notes

- The DS3231 retains time through power-cycles using its onboard coin cell. Replace the cell if the RTC resets unexpectedly.
- `lastGiven` for PRN medications is stored in RAM only and resets on reboot. The SD log remains intact.
- Increase `MAX_SD_LOGS` in `types.h` if you need a longer history. Each extra entry costs 64 bytes of SD space.
