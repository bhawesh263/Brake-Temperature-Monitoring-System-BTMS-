#include "../include/MovingAverage.h"

MovingAverage::MovingAverage(int size) {
    windowSize = size;
    buffer = new float[windowSize]{0}; // Initialize buffer with zeros
    insertIndex = 0;
    sum = 0.0f;
    count = 0;
}

MovingAverage::~MovingAverage() {
    delete[] buffer; // Free memory to prevent memory leaks
}

float MovingAverage::process(float newReading) {
    // 1. Subtract the oldest reading from our running total
    sum -= buffer[insertIndex];
    
    // 2. Insert the new reading into the buffer and add to total
    buffer[insertIndex] = newReading;
    sum += newReading;
    
    // 3. Move the index forward. If it hits the end, wrap around to 0.
    insertIndex = (insertIndex + 1) % windowSize;
    
    // 4. Track how many readings we've taken until the buffer is full
    if (count < windowSize) {
        count++;
    }
    
    return getAverage();
}

float MovingAverage::getAverage() const {
    if (count == 0) return 0.0f;
    return sum / count;
}