#pragma once
#include <MFRC522.h>

// Read the UID of a card currently in field. Returns an uppercase hex string
// like "AB 12 CD 34", or an empty string if no card is present.
String readUID(MFRC522& reader);

// Search for a matching medication by UID. Returns the index into medications[],
// or -1 if the UID is not in the list.
int findMedication(const String& uid);
