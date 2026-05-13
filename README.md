# EmbedSim: Enterprise IoT Condition Monitoring System

EmbedSim is a real-time industrial IoT monitoring platform designed to simulate, ingest, and visualize telemetry data from distributed edge nodes. It features a high-performance C++ edge simulation, a scalable Go telemetry hub, and a premium real-time analytics dashboard.

## 🚀 System Architecture

- **Edge Node (C++)**: Simulates industrial sensor data with built-in signal processing (Moving Average filter) and state detection.
- **Telemetry Hub (Go)**: A high-concurrency ingestion engine that manages machine states, history tracking, and persistence.
- **Analytics Dashboard (HTML5/Chart.js)**: A glassmorphic, real-time interface for fleet-wide monitoring and anomaly visualization.
- **Infrastructure**: Fully containerized using Docker and Docker Compose.

## 🛠️ Tech Stack

- **Firmware Sim**: C++17, CMake
- **Backend**: Go (Golang), Redis
- **Frontend**: Vanilla JS, Chart.js, CSS3 (Glassmorphism)
- **DevOps**: Docker, Docker Compose

## 📦 Getting Started

### Prerequisites
- Docker & Docker Compose
- *Alternative for local run*: Go 1.20+, CMake 3.10+, GCC/Clang

### One-Command Deployment (Docker)
```bash
docker-compose up --build
```
Access the dashboard at `http://localhost/dashboard.html` (or open the local file if not served by a web container).

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

3. **View Dashboard**:
   Open `dashboard.html` in any modern browser.

## 📊 Features

- **Signal Filtering**: Real-time noise reduction using moving average algorithms.
- **State Machine**: Intelligent transition between NORMAL, WARNING, and CRITICAL states based on processed telemetry.
- **Persistent History**: The hub automatically saves session data to `history.json` and supports Redis for enterprise-grade persistence.
- **Multi-Node Support**: Simulate an entire fleet of machines by running multiple instances of the monitor with unique IDs.

