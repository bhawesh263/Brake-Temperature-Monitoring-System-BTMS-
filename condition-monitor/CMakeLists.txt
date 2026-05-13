cmake_minimum_required(VERSION 3.10)

# Name of our project
project(ConditionMonitor VERSION 1.0)

# Specify that we are using C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the 'include' directory so the compiler can find our header files
include_directories(include)

# Create the executable from our source files
add_executable(monitor_sim 
    src/main.cpp 
    src/StateMachine.cpp 
    src/MovingAverage.cpp
)