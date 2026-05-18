#include "../include/PredictiveModel.h"

PredictiveModel::PredictiveModel(float lr, int ep, int size) 
    : m(0.0f), c(0.0f), learningRate(lr), epochs(ep), windowSize(size), dataCount(0), writeIndex(0) {
    if (windowSize > 20) windowSize = 20;
}

void PredictiveModel::addDataPoint(float time, float temp) {
    xData[writeIndex] = time;
    yData[writeIndex] = temp;
    writeIndex = (writeIndex + 1) % windowSize;
    if (dataCount < windowSize) dataCount++;
    
    train();
}

void PredictiveModel::train() {
    if (dataCount < 2) return;
    
    int oldestIndex = (dataCount == windowSize) ? writeIndex : 0;
    float x0 = xData[oldestIndex];
    
    for (int e = 0; e < epochs; e++) {
        float dm = 0;
        float dc = 0;
        
        for (int i = 0; i < dataCount; i++) {
            int idx = (oldestIndex + i) % windowSize;
            float x = xData[idx] - x0;
            float y = yData[idx];
            float y_pred = m * x + c;
            float error = y_pred - y;
            
            dm += error * x;
            dc += error;
        }
        dm = (2.0f / dataCount) * dm;
        dc = (2.0f / dataCount) * dc;
        
        m -= learningRate * dm;
        c -= learningRate * dc;
    }
}

float PredictiveModel::predict(float futureTime) {
    if (dataCount == 0) return 0.0f;
    int oldestIndex = (dataCount == windowSize) ? writeIndex : 0;
    float x0 = xData[oldestIndex];
    return m * (futureTime - x0) + c;
}

float PredictiveModel::getSlope() const { return m; }
float PredictiveModel::getIntercept() const { return c; }
