// File: examples/ReadTime/ReadTime.ino

#include <Wire.h>
#include "FRTC8900.h"

FRTC8900 rtc;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC! Check wiring.");
    while (1);
  }

  if (!rtc.isTimeValid()) {
    Serial.println("RTC has lost power. Time is not set!");
    Serial.println("Please run the SetTimeWithCompiler example to set the time.");
  }
}

void loop() {
  uint8_t sec, min, hour, dayOfWeek, dayOfMonth, month;
  uint16_t year;

  rtc.getTime(sec, min, hour, dayOfWeek, dayOfMonth, month, year);

  Serial.print(year);
  Serial.print("/");
  printTwoDigits(month);
  Serial.print("/");
  printTwoDigits(dayOfMonth);
  Serial.print(" (");
  printDayOfWeek(dayOfWeek);
  Serial.print(") ");
  printTwoDigits(hour);
  Serial.print(":");
  printTwoDigits(min);
  Serial.print(":");
  printTwoDigits(sec);
  Serial.println();

  delay(1000);
}

void printTwoDigits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}

void printDayOfWeek(uint8_t day) {
  switch (day) {
    case 1: Serial.print("Sun"); break;
    case 2: Serial.print("Mon"); break;
    case 3: Serial.print("Tue"); break;
    case 4: Serial.print("Wed"); break;
    case 5: Serial.print("Thu"); break;
    case 6: Serial.print("Fri"); break;
    case 7: Serial.print("Sat"); break;
    default: Serial.print("???"); break;
  }
}