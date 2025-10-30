// File: examples/SetTimeWithCompiler/SetTimeWithCompiler.ino

#include <Wire.h>
#include "FRTC8900.h"

FRTC8900 rtc;

// Function to parse the __DATE__ and __TIME__ macros
void setRtcToCompilerTime() {
  char dateStr[] = __DATE__; // "Mmm dd yyyy"
  char timeStr[] = __TIME__; // "hh:mm:ss"

  // Parse date
  char monthStr[4];
  int day, year;
  sscanf(dateStr, "%s %d %d", monthStr, &day, &year);

  int month;
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  for (int i = 0; i < 12; i++) {
    if (strcmp(monthStr, months[i]) == 0) {
      month = i + 1;
      break;
    }
  }

  // Parse time
  int hour, min, sec;
  sscanf(timeStr, "%d:%d:%d", &hour, &min, &sec);

  // Note: Day of week is not provided by the compiler macros, so we set it to 1 (Sunday) as a placeholder.
  // You can set it manually if needed.
  rtc.setTime(sec, min, hour, 1, day, month, year);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    while (1);
  }

  Serial.println("Setting RTC to compiler time...");
  setRtcToCompilerTime();
  Serial.println("RTC time set. Please upload the ReadTime example to verify.");
}

void loop() {
  // Nothing to do here
}