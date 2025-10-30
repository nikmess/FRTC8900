# Arduino Library for the NYFEA FRTC8900 RTC

![Library Version](https://img.shields.io/badge/version-1.1.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

This is a comprehensive Arduino library for the NYFEA FRTC8900 I2C Real-Time Clock module. It provides an easy-to-use interface for all major features of the chip, including timekeeping, alarms, and periodic interrupts.

## Features

*   **Full Timekeeping:** Set and get the Year, Month, Day, Day of Week, Hour, Minute, and Second.
*   **Automatic BCD Conversion:** All time-related functions use standard decimal numbers; the library handles the Binary-Coded Decimal (BCD) conversion automatically.
*   **Alarm Interrupt:** Set a daily alarm based on minute, hour, and day (either day-of-week or day-of-month). The `/INT` pin signals when the alarm triggers.
*   **Periodic Update Interrupt:** Configure a periodic interrupt on the `/INT` pin that can fire every second or every minute, perfect for synchronizing tasks or waking a microcontroller from sleep.
*   **Power Loss Detection:** A function to check the Voltage Low Flag (VLF) to determine if the RTC has lost power and if the time data is reliable.
*   **Frequency Output:** Control the square-wave output frequency on the `FOUT` pin (32.768 kHz, 1024 Hz, or 1 Hz).
*   **Temperature Sensing:** Read the raw 8-bit value from the internal temperature sensor.

## Hardware Connection

You only need four wires for basic I2C communication.

| FRTC8900 Pin | Arduino Pin      | Description                                |
|--------------|------------------|--------------------------------------------|
| `VDD`        | 5V or 3.3V       | Main Power Supply                          |
| `GND`        | GND              | Ground                                     |
| `SCL`        | SCL (A5 on Uno)  | I2C Clock Line                             |
| `SDA`        | SDA (A4 on Uno)  | I2C Data Line                              |
| `VBAT`       | 3V Battery (+)   | Backup battery power                       |
| `/INT` (optional) | Digital Pin (e.g., 2) | Interrupt output pin (active low)          |
| `FOE` (optional)  | Digital Pin or VDD | Frequency Output Enable (active high)      |

**Note:** Pull-up resistors (typically 4.7kΩ) are required on the SDA and SCL lines. Many Arduino boards and RTC modules have these built-in.

## Installation

1.  **Using the Arduino Library Manager (Recommended):**
    *   (Once published) In the Arduino IDE, go to `Sketch` -> `Include Library` -> `Manage Libraries...`.
    *   Search for "FRTC8900" and click `Install`.

2.  **Manual Installation (from .zip):**
    *   Download this repository as a `.zip` file from the GitHub page.
    *   In the Arduino IDE, go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
    *   Select the downloaded `.zip` file.
    *   The library will now be available in the `File` -> `Examples` menu.

## Basic Usage

Here is a simple example of how to initialize the RTC, check if the time is valid, and print the current time to the Serial Monitor.

```cpp
#include <Wire.h>
#include "FRTC8900.h"

// Create an RTC object
FRTC8900 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC! Check wiring.");
    while (1);
  }

  // Check if the RTC has lost power. If so, the time is not reliable.
  if (!rtc.isTimeValid()) {
    Serial.println("RTC has lost power. Time is not set!");
    // You should set the time now, for example, using the SetTimeWithCompiler example.
    // Example: rtc.setTime(0, 30, 16, 2, 10, 10, 2024); // 16:30:00, Monday, Oct 10, 2024
  }
}

void loop() {
  // Declare variables to hold the time
  uint8_t sec, min, hour, dayOfWeek, dayOfMonth, month;
  uint16_t year;

  // Get the current time from the RTC
  rtc.getTime(sec, min, hour, dayOfWeek, dayOfMonth, month, year);

  // Print the time to the Serial Monitor
  Serial.print(year);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(dayOfMonth);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(min);
  Serial.print(":");
  Serial.print(sec);
  Serial.println();

  delay(1000);
}
```

## Included Examples

The library comes with several examples to demonstrate its features:
*   **ReadTime.ino:** Reads the current time and prints it to the serial port every second.
*   **SetTimeWithCompiler.ino:** A utility sketch to set the RTC's time based on the time your computer had when the sketch was compiled.
*   **AlarmInterrupt.ino:** Shows how to set a daily alarm that triggers an interrupt on an Arduino pin.
*   **UpdateInterrupt.ino:** Demonstrates how to configure a periodic interrupt that fires every second.

## API Reference

A brief overview of the main public functions:

### Timekeeping
*   `bool begin(TwoWire &wirePort = Wire)`: Initializes the I2C communication.
*   `void setTime(...)`: Sets the date and time.
*   `void getTime(...)`: Reads the current date and time.
*   `bool isTimeValid()`: Checks if the clock data is reliable (has not lost power).
*   `void clearVoltageLowFlag()`: Clears the power loss flag after setting the time.

### Alarm
*   `void setAlarm(...)`: Configures the alarm time and mode (day of week vs. day of month).
*   `void enableAlarm()`: Enables the `/INT` pin output for alarms.
*   `void disableAlarm()`: Disables the `/INT` pin output for alarms.
*   `bool alarmFired()`: Checks if the alarm has occurred.
*   `void clearAlarmFlag()`: Clears the alarm flag after it has been handled.

### Update Interrupt
*   `void enableUpdateInterrupt(UpdateRate rate)`: Enables a periodic interrupt (every second or minute).
*   `void disableUpdateInterrupt()`: Disables the periodic interrupt.
*   `bool updateFired()`: Checks if a periodic interrupt has occurred.
*   `void clearUpdateFlag()`: Clears the periodic interrupt flag.

---

## Library Origin and Authorship

This library was generated by **Gemini 2.5 Pro**. (note from @nikmess: Including this README.md, except that it wanted to credit GPT-4 instead)

The entire library, including its functions, register maps, logic, and examples, was created solely based on the **`FRTC8900 Product Specification`** datasheet (Version 1.3, dated 2024/11/1 in the document) provided during a user interaction.

The accuracy and completeness of this library are therefore directly dependent on the accuracy and completeness of the provided datasheet. The AI generated the code by interpreting the technical specifications, register descriptions, and timing diagrams within that document.

## License

This library is released under the **MIT License**. See (https://opensource.org/license/mit) for more details.