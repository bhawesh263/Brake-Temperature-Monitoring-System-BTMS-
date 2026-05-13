#ifndef HAL_INTERFACES_H
#define HAL_INTERFACES_H

#include <vector>
#include <string>
#include <cstdint>

// Abstract interface for I2C (Sensors)
class II2CDriver {
public:
    virtual ~II2CDriver() = default;
    virtual bool init() = 0;
    virtual bool readRegister(uint8_t reg, uint8_t* data, uint8_t len) = 0;
    virtual bool writeRegister(uint8_t reg, uint8_t data) = 0;
};

// Abstract interface for CAN Bus
class ICANDriver {
public:
    virtual ~ICANDriver() = default;
    virtual bool init() = 0;
    virtual bool sendFrame(uint32_t id, const uint8_t* data, uint8_t len) = 0;
    virtual bool receiveFrame(uint32_t& id, uint8_t* data, uint8_t& len) = 0;
};

// Abstract interface for UART (Logging)
class IUARTDriver {
public:
    virtual ~IUARTDriver() = default;
    virtual void print(const std::string& msg) = 0;
    virtual void println(const std::string& msg) = 0;
};

#endif // HAL_INTERFACES_H
