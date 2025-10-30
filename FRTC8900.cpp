#include "FRTC8900.h"

FRTC8900::FRTC8900() {
    _i2cPort = nullptr;
}

bool FRTC8900::begin(TwoWire &wirePort) {
    _i2cPort = &wirePort;
    _i2cPort->begin();
    _i2cPort->beginTransmission(FRTC8900_I2C_ADDRESS);
    return (_i2cPort->endTransmission() == 0);
}

void FRTC8900::setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year) {
    _i2cPort->beginTransmission(FRTC8900_I2C_ADDRESS);
    _i2cPort->write(FRTC8900_REG_SEC);
    _i2cPort->write(decToBcd(sec));
    _i2cPort->write(decToBcd(min));
    _i2cPort->write(decToBcd(hour));
    // Day of week uses a bitmask, not BCD
    _i2cPort->write(1 << (dayOfWeek - 1));
    _i2cPort->write(decToBcd(dayOfMonth));
    _i2cPort->write(decToBcd(month));
    _i2cPort->write(decToBcd(year % 100));
    _i2cPort->endTransmission();

    clearVoltageLowFlag();
}

void FRTC8900::getTime(uint8_t &sec, uint8_t &min, uint8_t &hour, uint8_t &dayOfWeek, uint8_t &dayOfMonth, uint8_t &month, uint16_t &year) {
    _i2cPort->beginTransmission(FRTC8900_I2C_ADDRESS);
    _i2cPort->write(FRTC8900_REG_SEC);
    _i2cPort->endTransmission(false); // Send restart

    _i2cPort->requestFrom(FRTC8900_I2C_ADDRESS, 7);
    sec = bcdToDec(_i2cPort->read());
    min = bcdToDec(_i2cPort->read());
    hour = bcdToDec(_i2cPort->read());
    uint8_t week_reg = _i2cPort->read();
    dayOfMonth = bcdToDec(_i2cPort->read());
    month = bcdToDec(_i2cPort->read());
    year = 2000 + bcdToDec(_i2cPort->read());

    // Convert week bitmask back to a number
    for (int i = 0; i < 7; i++) {
        if ((week_reg >> i) & 1) {
            dayOfWeek = i + 1;
            break;
        }
    }
}

bool FRTC8900::isTimeValid() {
    uint8_t flagReg = readRegister(FRTC8900_REG_FLAG);
    return !((flagReg >> 1) & 1); // Check VLF bit (bit 1)
}

void FRTC8900::clearVoltageLowFlag() {
    uint8_t flagReg = readRegister(FRTC8900_REG_FLAG);
    flagReg &= ~(1 << 1); // Clear VLF bit
    writeRegister(FRTC8900_REG_FLAG, flagReg);
}

void FRTC8900::setAlarm(uint8_t min, uint8_t hour, uint8_t day, AlarmMode mode) {
    // Set WADA bit based on alarm mode
    updateRegisterBit(FRTC8900_REG_EXTENSION, 6, (mode == ALARM_DAY_OF_MONTH));

    // Write alarm registers. AE bit is 0 to enable that field.
    writeRegister(FRTC8900_REG_MIN_ALARM, decToBcd(min) & 0x7F); // AE=0
    writeRegister(FRTC8900_REG_HOUR_ALARM, decToBcd(hour) & 0x7F); // AE=0
    if (mode == ALARM_DAY_OF_WEEK) {
        writeRegister(FRTC8900_REG_WEEK_ALARM, (1 << (day - 1)) & 0x7F); // AE=0
    } else {
        writeRegister(FRTC8900_REG_WEEK_ALARM, decToBcd(day) & 0x7F); // AE=0
    }
}

void FRTC8900::enableAlarm() {
    updateRegisterBit(FRTC8900_REG_CONTROL, 3, true); // Set AIE bit
}

void FRTC8900::disableAlarm() {
    updateRegisterBit(FRTC8900_REG_CONTROL, 3, false); // Clear AIE bit
}

bool FRTC8900::alarmFired() {
    return (readRegister(FRTC8900_REG_FLAG) >> 3) & 1; // Check AF bit
}

void FRTC8900::clearAlarmFlag() {
    uint8_t flagReg = readRegister(FRTC8900_REG_FLAG);
    flagReg &= ~(1 << 3); // Clear AF bit
    writeRegister(FRTC8900_REG_FLAG, flagReg);
}

/**************************************************************************/
/*!
    @brief  Enables the periodic update interrupt.
    @param  rate The desired rate for the interrupt (UPDATE_SECONDLY or UPDATE_MINUTELY).
*/
/**************************************************************************/
void FRTC8900::enableUpdateInterrupt(UpdateRate rate) {
    // Set the USEL bit (bit 5) in the Extension register for second/minute rate
    updateRegisterBit(FRTC8900_REG_EXTENSION, 5, rate);
    // Set the UIE bit (bit 5) in the Control register to enable interrupt output
    updateRegisterBit(FRTC8900_REG_CONTROL, 5, true);
}

/**************************************************************************/
/*!
    @brief  Disables the periodic update interrupt.
*/
/**************************************************************************/
void FRTC8900::disableUpdateInterrupt() {
    // Clear the UIE bit (bit 5) in the Control register
    updateRegisterBit(FRTC8900_REG_CONTROL, 5, false);
}

/**************************************************************************/
/*!
    @brief  Checks if an update interrupt has occurred.
    @return True if the Update Flag (UF) is set, false otherwise.
*/
/**************************************************************************/
bool FRTC8900::updateFired() {
    // Check the UF bit (bit 5) in the Flag register
    return (readRegister(FRTC8900_REG_FLAG) >> 5) & 1;
}

/**************************************************************************/
/*!
    @brief  Clears the update interrupt flag.
*/
/**************************************************************************/
void FRTC8900::clearUpdateFlag() {
    uint8_t flagReg = readRegister(FRTC8900_REG_FLAG);
    // Clear the UF bit (bit 5)
    flagReg &= ~(1 << 5);
    writeRegister(FRTC8900_REG_FLAG, flagReg);
}

void FRTC8900::enableOutput() {
    // Note: The hardware FOE pin must be high for output to work.
    // This function only enables the internal generation.
    // The TE bit seems to be for the timer, not FOUT.
    // The datasheet implies FOUT is enabled when FOE pin is high.
    // The library's role is to set the frequency.
}

void FRTC8900::disableOutput() {
    // Note: To disable output, set the external FOE pin LOW.
}

void FRTC8900::setOutputFrequency(FOUT_Frequency freq) {
    uint8_t extReg = readRegister(FRTC8900_REG_EXTENSION);
    extReg &= ~0b00110000; // Clear FSEL1 and FSEL0 bits
    extReg |= (freq << 4);
    writeRegister(FRTC8900_REG_EXTENSION, extReg);
}

uint8_t FRTC8900::getTemperatureRaw() {
    return readRegister(FRTC8900_REG_TEMP);
}

// Private helper functions
uint8_t FRTC8900::bcdToDec(uint8_t val) {
    return ((val / 16) * 10) + (val % 16);
}

uint8_t FRTC8900::decToBcd(uint8_t val) {
    return ((val / 10) * 16) + (val % 10);
}

void FRTC8900::writeRegister(uint8_t regAddr, uint8_t data) {
    _i2cPort->beginTransmission(FRTC8900_I2C_ADDRESS);
    _i2cPort->write(regAddr);
    _i2cPort->write(data);
    _i2cPort->endTransmission();
}

uint8_t FRTC8900::readRegister(uint8_t regAddr) {
    _i2cPort->beginTransmission(FRTC8900_I2C_ADDRESS);
    _i2cPort->write(regAddr);
    _i2cPort->endTransmission(false); // Send restart
    _i2cPort->requestFrom(FRTC8900_I2C_ADDRESS, 1);
    return _i2cPort->read();
}

void FRTC8900::updateRegisterBit(uint8_t regAddr, uint8_t bit, bool value) {
    uint8_t regVal = readRegister(regAddr);
    if (value) {
        regVal |= (1 << bit);
    } else {
        regVal &= ~(1 << bit);
    }
    writeRegister(regAddr, regVal);
}