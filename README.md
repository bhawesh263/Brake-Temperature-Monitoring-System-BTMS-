# Brake Temperature Monitoring System (BTMS) with Edge AI

This project is a high-fidelity, safety-critical industrial IoT firmware simulation featuring an embedded TinyML/Edge AI model built in pure C++. It is designed to proactively detect and predict thermal runaway in automotive braking systems before critical failure occurs.

## 🚀 System Architecture

- **Edge Node (C++)**: Simulates high-performance sensor reading with moving average signal processing. Features an embedded Online Linear Regression engine (using Gradient Descent) to forecast brake temperatures 10 seconds into the future.
- **Telemetry Hub (Go)**: A high-concurrency ingestion engine that aggregates telemetry, machine states, and AI predictions.
- **Analytics Dashboard (HTML5/Chart.js)**: A dynamic, glassmorphic UI for tracking real-time fleet metrics and visualizing AI-predicted Time-To-Failure (TTF).
- **Infrastructure**: Fully containerized with Docker and Docker Compose.

## 🧠 Why Brake Thresholds Matter
In high-performance automotive systems, brake rotors routinely operate at elevated temperatures. The thresholds are physically modeled as follows:
- **`200°C` (WARNING)**: The brakes are hot, typical of heavy track use or continuous downhill braking.
- **`400°C` (CRITICAL)**: The critical failure point where brake fluid can boil and brake pads experience severe fade, leading to a total loss of stopping power.

The Embedded AI model tracks the *slope* of the temperature curve. If the temperature is rapidly rising and the AI predicts the brake will hit `400°C` in the next 10 seconds, it will fire a `PREDICTIVE_CRITICAL` alert on the CAN bus, allowing the vehicle's ECU to proactively engage regenerative braking or warn the driver *before* stopping power is lost.

## 🛠️ Tech Stack

- **Firmware Sim**: C++17, CMake (No heavy ML libraries used; pure math arrays)
- **Backend**: Go (Golang), Redis
- **Frontend**: Vanilla JS, Chart.js, CSS3 (Glassmorphism)
- **DevOps**: Docker, Docker Compose

## 📦 Getting Started

### One-Command Deployment
```bash
docker-compose up --build
```
Access the dashboard at `http://localhost/dashboard.html` (or open the local file).

### Local Manual Setup

1. **Start the Telemetry Hub**:
   ```bash
   cd telemetry-backend
   go run main.go
   ```

2. **Build and Run Edge Nodes**:
   ```bash
   cd condition-monitor
   mkdir build && cd build
   cmake ..
   make
   ./monitor_sim NODE-01
   ```

## 📊 Features

- **Embedded AI Predictive Engine**: Real-time linear regression using gradient descent directly in C++ to forecast sensor values.
- **State Machine**: Intelligent transition between NORMAL, WARNING, PREDICTIVE_CRITICAL, and CRITICAL states.
- **Signal Filtering**: Real-time noise reduction using moving average algorithms.
- **Fault-Tolerant Watchdog**: A software watchdog that guarantees real-time execution limits.
- **Persistent History**: The hub automatically saves session data to `history.json`.

---
*Developed as a demonstration of safety-critical embedded AI systems architecture.*
