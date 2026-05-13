#include "StateMachine.h"

StateMachine::StateMachine(float warnLimit, float errorLimit) {
    warningThreshold = warnLimit;
    errorThreshold = errorLimit;
    currentState = SystemState::NORMAL;
}

void StateMachine::update(float sensorValue) {
    if (sensorValue >= errorThreshold) {
        currentState = SystemState::ERROR;
    } 
    else if (sensorValue >= warningThreshold) {
        currentState = SystemState::WARNING;
    } 
    else {
        currentState = SystemState::NORMAL;
    }
}

std::string StateMachine::getStateName() const {
    switch (currentState) {
        case SystemState::NORMAL:  return "NORMAL";
        case SystemState::WARNING: return "WARNING";
        case SystemState::ERROR:   return "CRITICAL";
        default:                   return "UNKNOWN";
    }
}