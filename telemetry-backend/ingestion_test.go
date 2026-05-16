package main

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"
)

func TestIngestionHandler(t *testing.T) {
	payload := SensorPayload{
		MachineID:     "TEST-01",
		RawValue:      25.5,
		FilteredValue: 25.5,
		SystemState:   "NORMAL",
	}
	body, _ := json.Marshal(payload)

	req, err := http.NewRequest("POST", "/ingest", bytes.NewBuffer(body))
	if err != nil {
		t.Fatal(err)
	}

	rr := httptest.NewRecorder()
	handler := http.HandlerFunc(IngestionHandler)

	handler.ServeHTTP(rr, req)

	if status := rr.Code; status != http.StatusOK {
		t.Errorf("handler returned wrong status code: got %v want %v",
			status, http.StatusOK)
	}

	dataMutex.RLock()
	defer dataMutex.RUnlock()
	if _, ok := machines["TEST-01"]; !ok {
		t.Errorf("expected machine TEST-01 to be in memory")
	}
}

func TestHealthHandler(t *testing.T) {
	req, _ := http.NewRequest("GET", "/health", nil)
	rr := httptest.NewRecorder()
	handler := http.HandlerFunc(HealthHandler)

	handler.ServeHTTP(rr, req)

	if status := rr.Code; status != http.StatusOK {
		t.Errorf("handler returned wrong status code: got %v want %v",
			status, http.StatusOK)
	}
}
