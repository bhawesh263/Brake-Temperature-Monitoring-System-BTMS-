#include "../include/StateMachine.h"
#include <string>

StateMachine::StateMachine(float warnLimit, float errorLimit) {
    warningThreshold = warnLimit;
    errorThreshold = errorLimit;
    currentState = SystemState::NORMAL;
}

void StateMachine::update(float sensorValue, float predictedValue) {
    if (sensorValue >= errorThreshold) {
        currentState = SystemState::ERROR;
    } 
    else if (sensorValue >= warningThreshold) {
        currentState = SystemState::WARNING;
    } 
    else if (predictedValue >= errorThreshold) {
        currentState = SystemState::PREDICTIVE_CRITICAL;
    }
    else {
        currentState = SystemState::NORMAL;
    }
}

std::string StateMachine::getStateName() const {
    switch (currentState) {
        case SystemState::NORMAL:  return "NORMAL";
        case SystemState::WARNING: return "WARNING";
        case SystemState::PREDICTIVE_CRITICAL: return "PREDICTIVE_CRITICAL";
        case SystemState::ERROR:   return "CRITICAL";
        default:                   return "UNKNOWN";
    }
}