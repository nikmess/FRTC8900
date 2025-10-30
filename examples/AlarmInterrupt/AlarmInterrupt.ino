// File: examples/AlarmInterrupt/AlarmInterrupt.ino

#include <Wire.h>
#include "FRTC8900.h"

FRTC8900 rtc;

// Connect the RTC's /INT pin to this Arduino pin
const int INTERRUPT_PIN = 2;

// A flag to be set by the ISR
volatile bool alarmTriggered = false;

// Interrupt Service Routine
void onAlarm() {
  alarmTriggered = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), onAlarm, FALLING);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    while (1);
  }

  // Make sure time is set before setting an alarm
  if (!rtc.isTimeValid()) {
    Serial.println("RTC time is not set. Setting it now...");
    // Set to a known time: Jan 1, 2024, 12:00:00, Monday
    rtc.setTime(0, 0, 12, 2, 1, 1, 2024);
  }

  // Set an alarm for 10 seconds from now
  uint8_t sec, min, hour, dayOfWeek, dayOfMonth, month;
  uint16_t year;
  rtc.getTime(sec, min, hour, dayOfWeek, dayOfMonth, month, year);

  uint8_t alarm_min = min;
  if (sec >= 50) { // Handle minute rollover
      alarm_min = (min + 1) % 60;
  }
  
  Serial.print("Setting alarm for minute: ");
  Serial.println(alarm_min);
  
  // Set an alarm matching the minute. The hour and day are ignored by making their AE bits 1
  // Our library handles setting AE bits correctly, so we set a specific minute/hour/day alarm.
  // This will trigger once per day at this time.
  rtc.setAlarm(alarm_min, hour, dayOfMonth, ALARM_DAY_OF_MONTH);
  rtc.enableAlarm(); // This enables the /INT pin output
  
  Serial.println("Alarm is set. Waiting for it to trigger...");
}

void loop() {
  if (alarmTriggered) {
    Serial.println(">>> ALARM! <<<");

    // Important: Clear the alarm flag on the RTC so it can trigger again
    rtc.clearAlarmFlag();

    // Reset our software flag
    alarmTriggered = false;

    // You could put your alarm action code here (e.g., wake from sleep, sound a buzzer)
    
    Serial.println("Alarm cleared. Waiting for the next one.");
  }
}