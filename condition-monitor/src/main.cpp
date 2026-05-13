#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "MovingAverage.h"
#include "StateMachine.h"
#include "drivers/VirtualDrivers.h"

namespace config {
    const uint32_t SAMPLE_RATE_MS = 100;
    const uint32_t CAN_BROADCAST_RATE_MS = 500;
    const uint32_t WATCHDOG_TIMEOUT_MS = 200;
    const uint8_t MAX_SENSOR_FAILURES = 3;
}

class SafetyMonitorApp {
private:
    VirtualI2CSensor sensor;
    VirtualCANDriver canBus;
    MovingAverage filter;
    StateMachine stateMachine;
    
    uint32_t lastCanBroadcast = 0;
    uint32_t lastWatchdogKick = 0;
    uint8_t sensorFailureCount = 0;
    bool isSystemInSafeMode = false;
    uint32_t loopCounter = 0;

    void kickWatchdog() {
        lastWatchdogKick = 0;
    }

    bool checkSafety() {
        return lastWatchdogKick <= config::WATCHDOG_TIMEOUT_MS;
    }

    void enterSafeMode() {
        if (!isSystemInSafeMode) {
            isSystemInSafeMode = true;
            uint8_t failData[2] = {0xEE, 0x0F};
            canBus.sendFrame(0x123, failData, 2);
        }
    }

public:
    SafetyMonitorApp() : filter(5), stateMachine(80.0f, 120.0f) {}

    void run() {
        if (!sensor.init() || !canBus.init()) return;

        while (true) {
            if (loopCounter == 50) sensor.triggerFault();

            if (!isSystemInSafeMode) {
                uint8_t rawData[2];
                if (sensor.readRegister(0x00, rawData, 2)) {
                    sensorFailureCount = 0;
                    
                    int16_t rawVal = (rawData[0] << 8) | rawData[1];
                    float tempC = rawVal / 100.0f;
                    float filteredTemp = filter.process(tempC);
                    stateMachine.update(filteredTemp);

                    if (lastCanBroadcast >= config::CAN_BROADCAST_RATE_MS) {
                        uint8_t canData[2];
                        canData[0] = static_cast<uint8_t>(filteredTemp);
                        canData[1] = (stateMachine.getStateName() == "NORMAL") ? 0 : 
                                     (stateMachine.getStateName() == "WARNING") ? 1 : 2;
                        canBus.sendFrame(0x123, canData, 2);
                        lastCanBroadcast = 0;
                    }
                } else {
                    if (++sensorFailureCount >= config::MAX_SENSOR_FAILURES) {
                        enterSafeMode();
                    }
                }
            } else {
                if (lastCanBroadcast >= config::CAN_BROADCAST_RATE_MS) {
                    uint8_t canData[2] = {0xEE, 0x0F};
                    canBus.sendFrame(0x123, canData, 2);
                    lastCanBroadcast = 0;
                }
            }

            kickWatchdog();
            if (!checkSafety()) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(config::SAMPLE_RATE_MS));
            lastCanBroadcast += config::SAMPLE_RATE_MS;
            lastWatchdogKick += config::SAMPLE_RATE_MS;
            loopCounter++;
        }
    }
};

int main() {
    SafetyMonitorApp app;
    app.run();
    return 0;
}