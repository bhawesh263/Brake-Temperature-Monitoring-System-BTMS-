# Predictive AI Architecture Walkthrough

## 1. The Problem Space
In high-performance automotive environments (like racing or heavy-duty towing), brake rotors can experience extreme thermal loads. 
- A **Warning State (`200°C`)** means the brakes are getting hot, which is expected during aggressive driving.
- A **Critical State (`400°C`)** means the brakes are physically failing. Brake fluid boils, brake pads glaze, and the vehicle completely loses stopping power (brake fade).

**The flaw with traditional monitoring:** If you wait until the sensor actually reads `400°C` to fire a critical alert, it's already too late. The driver has lost the ability to stop. 

## 2. The Architecture Solution

> **Note on the platform:** this firmware runs as a host-compiled simulation using virtual I2C/CAN drivers. The algorithms, fixed-size buffers, and CPU budget reasoning are designed for portability to a Cortex-M3 target.

We implemented a **Proactive Edge AI Engine** directly in the firmware (C++).

Instead of relying on heavy Python libraries like TensorFlow or PyTorch, we wrote a lightweight **Online Linear Regression algorithm using Gradient Descent**.
- The C++ Edge Node keeps a rolling circular buffer (fixed `float[20]`) of the last 20 temperature readings.
- Every 100ms, the firmware recalculates the `slope` of the temperature curve using Gradient Descent.
- It then mathematically projects the temperature **10 seconds into the future**.
- If the projected temperature exceeds `400°C`, the node instantly fires a **`PREDICTIVE_CRITICAL`** state across the CAN bus.

This allows the vehicle's ECU to intercept the failure *before* it happens, giving the vehicle 10 seconds to automatically engage regenerative braking, engine braking, or forcefully alert the driver.

## 3. Live Demo: The 30-Second Fault Injection
To prove the AI works, we implemented an aggressive fault injection scenario in the C++ firmware.

**How to see it in action:**
1. Run the system: `docker-compose up --build`
2. Open the `dashboard.html` interface.
3. **The Timeline:**
   - **0s - 10s:** The system runs normally at around 25°C.
   - **10s:** The fault injection triggers. The temperature begins to ramp aggressively by 25°C every second.
   - **16s:** The physical temperature is only around `175°C` (NORMAL state). However, the AI calculates the steep slope and projects that in 10 seconds, the temperature will hit `425°C`. The AI fires the **`PREDICTIVE_CRITICAL`** alert. The dashboard card flashes purple.
   - **26s:** The physical temperature finally crosses `400°C`. The standard rule-based system finally fires the `CRITICAL` alert.

**Result:** The AI caught the runaway thermal event a full 10 seconds before the rule-based legacy system did.

## 4. Trade-offs in Model Design
When designing embedded AI for microcontrollers, engineering trade-offs must be made:

1. **Analytical Least Squares vs. Gradient Descent:**
   - Closed-form least squares is actually cheaper and exact for this 2-parameter case — roughly 100 multiply-adds per window vs. ~4,000 for 50 epochs of gradient descent. I chose gradient descent for three deliberate reasons: (1) the epoch count is a tunable knob that gives a deterministic worst-case CPU budget per sample, (2) `m` and `c` warm-start from the previous step, so the model converges in far fewer epochs in steady state than the worst case suggests, and (3) the same code structure extends to multi-feature or non-linear models without rewriting the math layer. If this ever became a CPU bottleneck on a Cortex-M0, the fallback is a 20-line closed-form path.
2. **Fixed Arrays vs. Dynamic Memory (`std::vector`):**
   - We replaced dynamic C++ vectors with fixed `float[20]` circular buffers. Dynamic memory allocation on an embedded heap leads to fragmentation and eventual HardFaults. Fixed buffers guarantee deterministic memory usage.
3. **Window Size (20 samples):**
   - A larger window (e.g., 100 samples) would smooth out noise but make the model react too slowly to sudden temperature spikes. A smaller window (e.g., 5 samples) would react instantly but be too susceptible to EMI noise on the I2C bus, causing false positives. 20 samples (representing 2 seconds of data) provides a reasonable balance of responsiveness and stability.
