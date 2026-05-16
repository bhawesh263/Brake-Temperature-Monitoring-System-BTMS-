#include <iostream>
#include <cassert>
#include "../include/MovingAverage.h"

void testMovingAverage() {
    MovingAverage filter(3);

    // Test case 1: Single value
    float val1 = filter.process(10.0f);
    assert(val1 == 10.0f);

    // Test case 2: Two values
    float val2 = filter.process(20.0f);
    assert(val2 == 15.0f); // (10 + 20) / 2

    // Test case 3: Full window
    float val3 = filter.process(30.0f);
    assert(val3 == 20.0f); // (10 + 20 + 30) / 3

    // Test case 4: Window shift (FIFO)
    float val4 = filter.process(40.0f);
    assert(val4 == 30.0f); // (20 + 30 + 40) / 3

    std::cout << "MovingAverage Unit Tests Passed!" << std::endl;
}

int main() {
    try {
        testMovingAverage();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
