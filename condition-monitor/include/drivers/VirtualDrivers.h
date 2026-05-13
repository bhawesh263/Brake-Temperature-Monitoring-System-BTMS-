#ifndef VIRTUAL_DRIVERS_H
#define VIRTUAL_DRIVERS_H

#include "HAL_Interfaces.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <sstream>

class VirtualI2CSensor : public II2CDriver {
private:
    float currentTemp;
    bool isFaulty;
public:
    VirtualI2CSensor() : currentTemp(25.0f), isFaulty(false) { srand(time(0)); }
    void triggerFault() { isFaulty = true; }
    bool init() override { return true; }

    bool readRegister(uint8_t reg, uint8_t* data, uint8_t len) override {
        if (isFaulty) return false;
        
        currentTemp += (rand() % 100 - 50) / 100.0f;
        if (rand() % 20 == 0) currentTemp += 10.0f;
        else currentTemp = currentTemp * 0.95f + 25.0f * 0.05f;

        int16_t raw = static_cast<int16_t>(currentTemp * 100);
        data[0] = (raw >> 8) & 0xFF;
        data[1] = raw & 0xFF;
        return true;
    }
    bool writeRegister(uint8_t reg, uint8_t data) override { return true; }
};

class VirtualCANDriver : public ICANDriver {
public:
    bool init() override { return true; }

    bool sendFrame(uint32_t id, const uint8_t* data, uint8_t len) override {
        float temp = data[0];
        std::string state = "NORMAL";
        if (data[0] == 0xEE) state = "SAFE_FAIL";
        else if (data[1] == 1) state = "WARNING";
        else if (data[1] == 2) state = "CRITICAL";

        std::stringstream cmd;
        cmd << "curl -s -X POST http://localhost:8080/ingest "
            << "-H \"Content-Type: application/json\" "
            << "-d '{\"machine_id\": \"CAN-NODE-01\", \"raw_value\": " << temp 
            << ", \"filtered_value\": " << temp << ", \"system_state\": \"" << state << "\"}' > /dev/null";
        system(cmd.str().c_str());
        return true;
    }

    bool receiveFrame(uint32_t& id, uint8_t* data, uint8_t& len) override { return false; }
};

#endif
