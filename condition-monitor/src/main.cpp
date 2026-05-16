#include <thread>
#include <chrono>
#include <cstdlib>
#include "../include/MovingAverage.h"
#include "../include/StateMachine.h"
#include "../include/Utils.h"
#include "../include/drivers/VirtualDrivers.h"

namespace config {
    const uint32_t SAMPLE_RATE_MS = 100;
    const uint32_t CAN_BROADCAST_RATE_MS = 500;
    const uint32_t WATCHDOG_TIMEOUT_MS = 200;
    const uint8_t MAX_SENSOR_FAILURES = 3;
    // TODO: Move these to a .json config file or environment variables for dynamic scaling
}

class SafetyMonitorApp {
private:
    std::string nodeId;
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
            // Notify other nodes on the CAN bus about the failure
            uint8_t failData[4] = {0xEE, 0xEE, 0x0F, 0x00};
            failData[3] = utils::calculateCRC8(failData, 3);
            canBus.sendFrame(nodeId, 0x123, failData, 4);
        }
    }

    bool performPOST() {
        // Power-On Self Test: Simulating RAM/Flash integrity check (Safety Req SR-03)
        // FIXME: Implement actual checksum verification for the firmware image
        return true; 
    }

public:
    SafetyMonitorApp(std::string id) : nodeId(id), filter(5), stateMachine(80.0f, 120.0f) {}

    void run() {
        if (!performPOST()) return;
        if (!sensor.init() || !canBus.init()) return;

        while (true) {
            // Optional: Uncomment for fault-injection testing
            // if (loopCounter == 50) sensor.triggerFault();


            if (!isSystemInSafeMode) {
                uint8_t rawData[2];
                if (sensor.readRegister(0x00, rawData, 2)) {
                    sensorFailureCount = 0;
                    
                    int16_t rawVal = (rawData[0] << 8) | rawData[1];
                    float tempC = rawVal / 100.0f;
                    float filteredTemp = filter.process(tempC);
                    stateMachine.update(filteredTemp);

                    if (lastCanBroadcast >= config::CAN_BROADCAST_RATE_MS) {
                        uint8_t canData[4];
                        canData[0] = static_cast<uint8_t>(tempC);
                        canData[1] = static_cast<uint8_t>(filteredTemp);
                        
                        // Map internal state to CAN status byte
                        canData[2] = (stateMachine.getStateName() == "NORMAL") ? 0 : 
                                     (stateMachine.getStateName() == "WARNING") ? 1 : 2;
                        
                        canData[3] = utils::calculateCRC8(canData, 3);
                        canBus.sendFrame(nodeId, 0x123, canData, 4);
                        lastCanBroadcast = 0;
                    }
                } else {
                    if (++sensorFailureCount >= config::MAX_SENSOR_FAILURES) {
                        enterSafeMode();
                    }
                }
            } else {
                if (lastCanBroadcast >= config::CAN_BROADCAST_RATE_MS) {
                    uint8_t canData[4] = {0xEE, 0xEE, 0x0F, 0x00};
                    canData[3] = utils::calculateCRC8(canData, 3);
                    canBus.sendFrame(nodeId, 0x123, canData, 4);
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

int main(int argc, char** argv) {
    std::string nodeId = "CAN-NODE-01";
    if (argc > 1) {
        nodeId = argv[1];
    }
    SafetyMonitorApp app(nodeId);
    app.run();
    return 0;
}