#ifndef VIRTUAL_DRIVERS_H
#define VIRTUAL_DRIVERS_H

#include "HAL_Interfaces.h"
#include "../Utils.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <string>
#include <sstream>
#include <cstdlib>

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

    bool sendFrame(const std::string& machineId, uint32_t id, const uint8_t* data, uint8_t len) override {
        // Verify CRC if present (SR-04)
        if (len >= 4) {
            uint8_t receivedCRC = data[3];
            uint8_t calculatedCRC = utils::calculateCRC8(data, 3);
            if (receivedCRC != calculatedCRC) {
                std::cerr << "[CAN BUS ERROR] CRC mismatch! Frame discarded." << std::endl;
                return false;
            }
        }

        float rawTemp = data[0];
        float filteredTemp = data[1];
        std::string state = "NORMAL";
        if (data[0] == 0xEE) state = "SAFE_FAIL";
        else if (data[2] == 1) state = "WARNING";
        else if (data[2] == 2) state = "CRITICAL";

        const char* backendUrl = std::getenv("BACKEND_URL");
        std::string url = backendUrl ? backendUrl : "http://localhost:8080/ingest";

        std::stringstream cmd;
        cmd << "curl -s -X POST " << url << " "
            << "-H \"Content-Type: application/json\" "
            << "-d '{\"machine_id\": \"" << machineId << "\", \"raw_value\": " << rawTemp 
            << ", \"filtered_value\": " << filteredTemp << ", \"system_state\": \"" << state << "\"}' > /dev/null";
        system(cmd.str().c_str());
        return true;
    }

    bool receiveFrame(uint32_t& id, uint8_t* data, uint8_t& len) override { return false; }
};

#endif
