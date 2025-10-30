// File: examples/UpdateInterrupt/UpdateInterrupt.ino

#include <Wire.h>
#include "FRTC8900.h"

FRTC8900 rtc;

// Connect the RTC's /INT pin to this Arduino pin
const int INTERRUPT_PIN = 2;

// A volatile flag to be set by the ISR
volatile bool secondTick = false;

// Interrupt Service Routine (ISR)
// This function is called every time the RTC's /INT pin goes low.
void onRtcUpdate() {
  secondTick = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Set up the interrupt pin
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), onRtcUpdate, FALLING);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC! Check wiring.");
    while (1);
  }

  // Ensure the time is set. If not, the interrupt might not fire correctly.
  if (!rtc.isTimeValid()) {
    Serial.println("RTC time is not set. Setting to a default time.");
    // Set to Jan 1, 2024, 12:00:00, Monday
    rtc.setTime(0, 0, 12, 2, 1, 1, 2024);
  }

  // Enable the update interrupt to fire every second.
  // The /INT pin will pulse low for ~7.8ms every second.
  // You can also use UPDATE_MINUTELY for a pulse every minute.
  rtc.enableUpdateInterrupt(UPDATE_SECONDLY);
  
  Serial.println("Update interrupt enabled. Waiting for ticks...");
}

void loop() {
  // Check if our interrupt flag has been set
  if (secondTick) {
    // According to the datasheet (page 20), the UF flag is set when the interrupt occurs.
    // We can confirm this before clearing it.
    if (rtc.updateFired()) {
      Serial.print("Tick! Time: ");
      
      uint8_t sec, min, hour, dayOfWeek, dayOfMonth, month;
      uint16_t year;
      rtc.getTime(sec, min, hour, dayOfWeek, dayOfMonth, month, year);
      Serial.println(sec);
      
      // IMPORTANT: You must clear the update flag in the RTC
      // so you can detect the next one.
      rtc.clearUpdateFlag();
    }
    
    // Reset our software flag for the next interrupt
    secondTick = false;
  }
  
  // You can put the microcontroller to sleep here to save power,
  // knowing it will wake up on the next interrupt from the RTC.
  // For example: sleep_mode();
}