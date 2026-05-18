#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string>

// These are the three possible states our system can be in
enum class SystemState {
    NORMAL,
    WARNING,
    PREDICTIVE_CRITICAL,
    ERROR
};

class StateMachine {
private:
    SystemState currentState;
    
    // Thresholds for triggering warnings or errors
    float warningThreshold;
    float errorThreshold;

public:
    // Constructor
    StateMachine(float warnLimit, float errorLimit);

    // Feed a sensor reading and predicted future reading into the state machine
    void update(float sensorValue, float predictedValue = 0.0f);

    // Get the current state as a readable word
    std::string getStateName() const;
};

#endif