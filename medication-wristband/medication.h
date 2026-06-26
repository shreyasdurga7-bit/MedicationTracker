#pragma once

// Possible outcomes when checking whether a medication should be given
enum MedStatus {
  MED_OK    = 0, // Safe to administer now
  MED_EARLY = 1, // Too early — scheduled window not yet reached, or PRN cooldown active
  MED_LATE  = 2  // Overdue — scheduled window has passed
};

// Check whether it is appropriate to give medications[idx] right now.
MedStatus checkMedication(int idx);
