#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string>

// These are the three possible states our system can be in
enum class SystemState {
    NORMAL,
    WARNING,
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

    // Feed a sensor reading into the state machine to update its state
    void update(float sensorValue);

    // Get the current state as a readable word
    std::string getStateName() const;
};

#endif