#include <Arduino.h>
#include "nfc.h"
#include "config.h"

String readUID(MFRC522& reader) {
  if (!reader.PICC_IsNewCardPresent() || !reader.PICC_ReadCardSerial()) {
    return "";
  }
  String uid = "";
  for (byte i = 0; i < reader.uid.size; i++) {
    if (i > 0) uid += " ";
    if (reader.uid.uidByte[i] < 0x10) uid += "0"; // zero-pad single hex digits
    uid += String(reader.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

int findMedication(const String& uid) {
  for (int i = 0; i < MAX_MEDICATIONS; i++) {
    if (uid == nfcUIDs[i]) return i;
  }
  return -1;
}
