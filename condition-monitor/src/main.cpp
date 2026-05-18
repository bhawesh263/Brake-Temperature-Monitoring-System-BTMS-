#include <thread>
#include <chrono>
#include <cstdlib>
#include "../include/MovingAverage.h"
#include "../include/StateMachine.h"
#include "../include/PredictiveModel.h"
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
    PredictiveModel aiModel;
    
    float timeSeconds = 0.0f;
    
    uint32_t lastCanBroadcast = 0;
    uint32_t lastWatchdogKick = 0;
    uint8_t sensorFailureCount = 0;
    bool isSystemInSafeMode = false;
    uint32_t loopCounter = 0;

    std::chrono::steady_clock::time_point lastWatchdogKickTime;

    void kickWatchdog() {
        lastWatchdogKickTime = std::chrono::steady_clock::now();
    }

    bool checkSafety() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastWatchdogKickTime).count();
        return elapsed <= config::WATCHDOG_TIMEOUT_MS;
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
    SafetyMonitorApp(std::string id) : nodeId(id), filter(5), stateMachine(200.0f, 400.0f), aiModel(0.005f, 50, 20) {
        kickWatchdog(); // Initialize watchdog time
    }

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
                    
                    // 30-second fault-injection scenario: aggressive thermal runaway
                    // Starts at 10 seconds (loopCounter 100), ramps up linearly
                    if (loopCounter >= 100 && loopCounter <= 400) {
                        tempC += (loopCounter - 100) * 2.5f; 
                    }
                    
                    float filteredTemp = filter.process(tempC);
                    
                    timeSeconds += config::SAMPLE_RATE_MS / 1000.0f;
                    aiModel.addDataPoint(timeSeconds, filteredTemp);
                    
                    // Predict 10 seconds into the future
                    float predictedTemp = aiModel.predict(timeSeconds + 10.0f);

                    stateMachine.update(filteredTemp, predictedTemp);

                    if (lastCanBroadcast >= config::CAN_BROADCAST_RATE_MS) {
                        uint8_t canData[4];
                        canData[0] = static_cast<uint8_t>(tempC);
                        canData[1] = static_cast<uint8_t>(filteredTemp);
                        
                        // Map internal state to CAN status byte
                        canData[2] = (stateMachine.getStateName() == "NORMAL") ? 0 : 
                                     (stateMachine.getStateName() == "WARNING") ? 1 : 
                                     (stateMachine.getStateName() == "PREDICTIVE_CRITICAL") ? 3 : 2;
                        
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

            if (!checkSafety()) break;
            kickWatchdog();

            std::this_thread::sleep_for(std::chrono::milliseconds(config::SAMPLE_RATE_MS));
            lastCanBroadcast += config::SAMPLE_RATE_MS;
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