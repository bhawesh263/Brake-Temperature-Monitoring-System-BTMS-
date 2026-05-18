#ifndef PREDICTIVE_MODEL_H
#define PREDICTIVE_MODEL_H


class PredictiveModel {
private:
    float m;
    float c;
    float learningRate;
    int epochs;
    int windowSize;
    float xData[20];
    float yData[20];
    int dataCount;
    int writeIndex;

public:
    PredictiveModel(float lr = 0.001f, int ep = 20, int size = 20);
    
    void addDataPoint(float time, float temp);
    void train();
    float predict(float futureTime);
    float getSlope() const;
    float getIntercept() const;
};

#endif
