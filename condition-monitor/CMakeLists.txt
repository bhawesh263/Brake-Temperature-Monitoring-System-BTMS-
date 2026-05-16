cmake_minimum_required(VERSION 3.10)

project(ConditionMonitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Main Application
add_executable(monitor_sim 
    src/main.cpp 
    src/StateMachine.cpp 
    src/MovingAverage.cpp
)
target_include_directories(monitor_sim PRIVATE include)

# Unit Test Target
add_executable(moving_average_test 
    src/moving_average_test.cpp 
    src/MovingAverage.cpp
)
target_include_directories(moving_average_test PRIVATE include)