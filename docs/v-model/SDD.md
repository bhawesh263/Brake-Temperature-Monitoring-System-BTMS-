# Software Design Document (SDD) - EmbedSim Safety Monitor

## 1. System Architecture
The system follows a Layered Architecture to ensure Hardware Abstraction (HAL) and modularity, supporting functional safety requirements.

### 1.1 Layer Diagram
- **Application Layer**: `SafetyMonitorApp` (High-level logic, state management).
- **Service Layer**: `MovingAverage`, `StateMachine` (Algorithmic processing).
- **HAL Layer**: `II2CDriver`, `ICANDriver`, `IUARTDriver` (Abstracted hardware access).
- **Hardware/Sim Layer**: `VirtualI2CSensor`, `VirtualCANDriver` (Protocol simulation).

## 2. Component Design

### 2.1 I2C Sensor Interface
- **Responsibility**: Fetching raw temperature data from the TMP117 sensor.
- **Error Handling**: Implements a timeout mechanism. If 3 consecutive reads fail, the system enters a Safe State.

### 2.2 CAN Communication
- **Format**: Standard 11-bit ID (0x123).
- **Data Payload**:
    - Byte 0: Filtered Temperature (integer).
    - Byte 1: System State (0=Normal, 1=Warning, 2=Critical).
- **Rate**: Deterministic 500ms broadcast.

### 2.3 Safety Mechanisms
- **Watchdog**: A logical watchdog that monitors loop execution time.
- **State Monotonicity**: Ensures that transitions between states follow defined safety paths.

## 3. Data Flow
1. **Clock Trigger**: Loop wakes every 100ms.
2. **HAL Read**: Request 2 bytes from I2C Register 0x00.
3. **Filtering**: Pass raw value to `MovingAverage`.
4. **State Update**: Pass filtered value to `StateMachine`.
5. **Output**: If 500ms elapsed, format CAN frame and send.
6. **Watchdog Kick**: Reset the watchdog timer.

---
**Prepared by**: Antigravity AI  
**Status**: Approved for Implementation
