#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

class MovingAverage {
private:
    int windowSize;
    float* buffer;
    int insertIndex;
    float sum;
    int count;

public:
    // Constructor and Destructor
    MovingAverage(int size);
    ~MovingAverage();

    // Core functionality
    float process(float newReading);
    float getAverage() const;
};

#endif