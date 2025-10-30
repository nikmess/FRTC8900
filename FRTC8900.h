#ifndef FRTC8900_H
#define FRTC8900_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address for the FRTC8900
#define FRTC8900_I2C_ADDRESS 0x32

// Register Map
#define FRTC8900_REG_SEC        0x00
#define FRTC8900_REG_MIN        0x01
#define FRTC8900_REG_HOUR       0x02
#define FRTC8900_REG_WEEK       0x03
#define FRTC8900_REG_DAY        0x04
#define FRTC8900_REG_MONTH      0x05
#define FRTC8900_REG_YEAR       0x06
#define FRTC8900_REG_MIN_ALARM  0x08
#define FRTC8900_REG_HOUR_ALARM 0x09
#define FRTC8900_REG_WEEK_ALARM 0x0A
#define FRTC8900_REG_EXTENSION  0x0D
#define FRTC8900_REG_FLAG       0x0E
#define FRTC8900_REG_CONTROL    0x0F
#define FRTC8900_REG_TEMP       0x17

// Frequency options for FOUT pin
enum FOUT_Frequency {
    FOUT_32768_HZ = 0b00,
    FOUT_1024_HZ  = 0b01,
    FOUT_1_HZ     = 0b10
    // 0b11 is also 32768Hz
};

// Day of week options for alarm
enum AlarmMode {
    ALARM_DAY_OF_WEEK = 0, // WADA bit = 0
    ALARM_DAY_OF_MONTH = 1  // WADA bit = 1
};

// Rate options for the update interrupt
enum UpdateRate {
    UPDATE_SECONDLY = 0, // USEL bit = 0
    UPDATE_MINUTELY = 1  // USEL bit = 1
};


class FRTC8900 {
public:
    FRTC8900();

    /**
     * @brief Initializes the I2C communication with the RTC.
     * @param wirePort The I2C interface to use (e.g., Wire, Wire1). Defaults to Wire.
     * @return True if the device is found and communication is established, false otherwise.
     */
    bool begin(TwoWire &wirePort = Wire);

    /**
     * @brief Sets the current time and date on the RTC.
     * @param sec Second (0-59)
     * @param min Minute (0-59)
     * @param hour Hour (0-23)
     * @param dayOfWeek Day of the week (1=Sunday, 2=Monday, ..., 7=Saturday)
     * @param dayOfMonth Day of the month (1-31)
     * @param month Month (1-12)
     * @param year Year (e.g., 2023)
     */
    void setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);

    /**
     * @brief Gets the current time and date from the RTC.
     * @param sec Reference to store the second.
     * @param min Reference to store the minute.
     * @param hour Reference to store the hour.
     * @param dayOfWeek Reference to store the day of the week.
     * @param dayOfMonth Reference to store the day of the month.
     * @param month Reference to store the month.
     * @param year Reference to store the year.
     */
    void getTime(uint8_t &sec, uint8_t &min, uint8_t &hour, uint8_t &dayOfWeek, uint8_t &dayOfMonth, uint8_t &month, uint16_t &year);

    /**
     * @brief Checks if the RTC time data is valid.
     * @details This checks the Voltage Low Flag (VLF). If true, the time is likely reliable.
     * If false, the RTC has likely lost power and the time needs to be reset.
     * @return True if time is valid, false otherwise.
     */
    bool isTimeValid();

    /**
     * @brief Clears the Voltage Low Flag (VLF). Call this after setting the time.
     */
    void clearVoltageLowFlag();

    /**
     * @brief Sets an alarm. The /INT pin will go low when the alarm triggers if enabled.
     * @param min Minute for the alarm (0-59).
     * @param hour Hour for the alarm (0-23).
     * @param day Day for the alarm (1-7 for day of week, 1-31 for day of month).
     * @param mode Selects whether the `day` parameter refers to the day of the week or day of the month.
     */
    void setAlarm(uint8_t min, uint8_t hour, uint8_t day, AlarmMode mode);

    /**
     * @brief Enables the alarm interrupt. The /INT pin will be driven low when an alarm occurs.
     */
    void enableAlarm();

    /**
     * @brief Disables the alarm interrupt.
     */
    void disableAlarm();

    /**
     * @brief Checks if the alarm has been triggered.
     * @return True if the Alarm Flag (AF) is set, false otherwise.
     */
    bool alarmFired();

    /**
     * @brief Clears the alarm flag. Must be called after an alarm has been handled.
     */
    void clearAlarmFlag();

    // Update Interrupt Functions
    /**
     * @brief Enables the periodic update interrupt. The /INT pin will pulse low at the specified rate.
     * @param rate The desired rate for the interrupt (UPDATE_SECONDLY or UPDATE_MINUTELY).
     */
    void enableUpdateInterrupt(UpdateRate rate = UPDATE_SECONDLY);
    
    /**
     * @brief Disables the periodic update interrupt.
     */
    void disableUpdateInterrupt();

    /**
     * @brief Checks if an update interrupt has occurred.
     * @return True if the Update Flag (UF) is set, false otherwise.
     */
    bool updateFired();

    /**
     * @brief Clears the update interrupt flag. Must be called after an update event to allow detection of the next one.
     */
    void clearUpdateFlag();


    /**
     * @brief Enables the square wave output on the FOUT pin.
     * @details The FOE pin must also be held HIGH externally for output to appear.
     */
    void enableOutput();

    /**
     * @brief Disables the square wave output on the FOUT pin.
     */
    void disableOutput();
    
    /**
     * @brief Sets the frequency of the square wave on the FOUT pin.
     * @param freq The desired frequency (FOUT_32768_HZ, FOUT_1024_HZ, or FOUT_1_HZ).
     */
    void setOutputFrequency(FOUT_Frequency freq);

    /**
     * @brief Reads the raw temperature data from the RTC.
     * @return The 8-bit raw temperature value from the register.
     * @note The datasheet does not provide a conversion formula to degrees Celsius.
     */
    uint8_t getTemperatureRaw();

private:
    TwoWire *_i2cPort;
    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
    void writeRegister(uint8_t regAddr, uint8_t data);
    uint8_t readRegister(uint8_t regAddr);
    void updateRegisterBit(uint8_t regAddr, uint8_t bit, bool value);
};

#endif