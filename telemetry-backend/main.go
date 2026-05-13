package main

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"sync"
	"time"
	"github.com/redis/go-redis/v9"
)

var ctx = context.Background()
var rdb *redis.Client

type SensorPayload struct {
	MachineID     string  `json:"machine_id"`
	RawValue      float64 `json:"raw_value"`
	FilteredValue float64 `json:"filtered_value"`
	SystemState   string  `json:"system_state"`
	Timestamp     int64   `json:"timestamp"`
}

type MachineStatus struct {
	ID            string          `json:"id"`
	CurrentState  string          `json:"current_state"`
	History       []SensorPayload `json:"history"`
}

var (
	dataMutex sync.RWMutex
	machines  = make(map[string]*MachineStatus)
)

const historyFile = "history.json"

func loadHistory() {
	data, err := os.ReadFile(historyFile)
	if err != nil {
		return
	}
	dataMutex.Lock()
	json.Unmarshal(data, &machines)
	dataMutex.Unlock()
}

func saveHistory() {
	dataMutex.RLock()
	data, _ := json.Marshal(machines)
	dataMutex.RUnlock()
	os.WriteFile(historyFile, data, 0644)
}

func IngestionHandler(w http.ResponseWriter, r *http.Request) {
	var payload SensorPayload
	if err := json.NewDecoder(r.Body).Decode(&payload); err != nil {
		http.Error(w, "Invalid JSON", http.StatusBadRequest)
		return
	}
	payload.Timestamp = time.Now().Unix()

	dataMutex.Lock()
	m, ok := machines[payload.MachineID]
	if !ok {
		m = &MachineStatus{ID: payload.MachineID}
		machines[payload.MachineID] = m
	}
	m.CurrentState = payload.SystemState
	m.History = append(m.History, payload)
	if len(m.History) > 30 {
		m.History = m.History[1:]
	}
	dataMutex.Unlock()

	if rdb != nil {
		stateData, _ := json.Marshal(payload)
		rdb.Set(ctx, payload.MachineID, stateData, 0)
	}

	w.WriteHeader(http.StatusOK)
}

func StatusHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	dataMutex.RLock()
	defer dataMutex.RUnlock()
	json.NewEncoder(w).Encode(machines)
}

func HealthHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("OK"))
}

func main() {
	loadHistory()
	
	go func() {
		for {
			time.Sleep(10 * time.Second)
			saveHistory()
		}
	}()

	redisAddr := os.Getenv("REDIS_ADDR")
	if redisAddr == "" {
		redisAddr = "localhost:6379"
	}

	rdb = redis.NewClient(&redis.Options{Addr: redisAddr})

	http.HandleFunc("/ingest", IngestionHandler)
	http.HandleFunc("/status", StatusHandler)
	http.HandleFunc("/health", HealthHandler)

	log.Printf("Server starting on :8080 (Redis: %s)", redisAddr)
	log.Fatal(http.ListenAndServe(":8080", nil))
}