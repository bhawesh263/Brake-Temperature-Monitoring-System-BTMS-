import requests
import time
import random
import argparse
import sys

def run_simulation(node_id, backend_url, interval):
    print(f"--- EmbedSim Virtual Node: {node_id} ---")
    print(f"Targeting: {backend_url}")
    print("Press Ctrl+C to stop simulation.\n")

    try:
        while True:
            # Simulate sensor drift and noise
            base_temp = 25.0
            noise = random.uniform(-2.0, 2.0)
            raw_val = base_temp + noise
            
            # Simple simulation logic
            filtered_val = raw_val * 0.9 + (base_temp * 0.1)
            
            state = "NORMAL"
            if raw_val > 28.5:
                state = "WARNING"

            payload = {
                "machine_id": node_id,
                "raw_value": round(raw_val, 2),
                "filtered_value": round(filtered_val, 2),
                "system_state": state
            }

            try:
                # Append /ingest to the base URL
                target = f"{backend_url.rstrip('/')}/ingest"
                response = requests.post(target, json=payload, timeout=2)
                if response.status_code == 200:
                    print(f"[{time.strftime('%H:%M:%S')}] {node_id} >> Data Sent | Val: {raw_val:.2f} | State: {state}")
                else:
                    print(f"Server returned error: {response.status_code}")
            except requests.exceptions.RequestException as e:
                print(f"Transport Error: {e}")

            time.sleep(interval)

    except KeyboardInterrupt:
        print("\nSimulation terminated by user.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="EmbedSim Virtual Edge Node Simulator")
    parser.add_argument("--id", default="VIRTUAL-NODE-01", help="Unique identifier for this node")
    parser.add_argument("--url", default="http://localhost:8080", help="Telemetry Hub base URL")
    parser.add_argument("--rate", type=float, default=1.0, help="Transmission rate in seconds")
    
    args = parser.parse_args()
    run_simulation(args.id, args.url, args.rate)
