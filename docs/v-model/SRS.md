# Software Requirements Specification (SRS) - EmbedSim Safety Monitor
**Project**: Brake Temperature Monitoring System (BTMS)  
**Safety Goal**: Prevent vehicle fire/brake failure due to overheating.  
**Integrity Level**: Targeted for ASIL-B (Automotive Safety Integrity Level).

---

## 1. Functional Requirements (FR)
| ID | Requirement | Description |
|---|---|---|
| FR-01 | Sensor Sampling | The system shall sample brake temperature via I2C every 100ms. |
| FR-02 | Data Filtering | The system shall apply a 5-point moving average filter to suppress EMI noise. |
| FR-03 | State Detection | The system shall identify NORMAL (<80°C), WARNING (80-120°C), and CRITICAL (>120°C) states. |
| FR-04 | Predictive AI Model | The system shall employ an Online Linear Regression model via Gradient Descent to predict future brake temperatures 10 seconds ahead and fire PREDICTIVE_CRITICAL warnings. |
| FR-05 | CAN Broadcast | The system shall broadcast the current state and raw values on the CAN Bus (ID: 0x123) every 500ms. |

## 2. Safety Requirements (SR)
| ID | Requirement | Description |
|---|---|---|
| SR-01 | Watchdog Timer | The system shall implement a hardware watchdog that resets the MCU if the main loop hangs for >200ms. |
| SR-02 | Communication Timeout | If the I2C sensor fails to respond for 3 consecutive samples, the system shall enter a "SAFE_FAIL" state. |
| SR-03 | Memory Integrity | The system shall perform a POST (Power-On Self-Test) for RAM and Flash integrity. |
| SR-04 | CRC Verification | All CAN messages shall include a 1-byte CRC for data integrity verification. |

## 3. Interface Requirements
| Interface | Protocol | Purpose |
|---|---|---|
| TMP117 | I2C | High-precision temperature sensor. |
| MCP2515 | SPI/CAN | External CAN controller for automotive bus communication. |
| Debug UART | UART | Logging and real-time diagnostic output. |

---
**Verified by**: Antigravity AI  
**Date**: 2026-05-13
