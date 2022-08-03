#ifndef KalmanFilter1D_h
#define KalmanFilter1D_h

#include "Arduino.h"

class KalmanFilter1D
{

public:
  KalmanFilter1D(float initialEstimation, float measurementVariance, float estimationVariance, float modelVariance);
  float updateEstimate(float measurement);

private:
  float measurementVariance; // r, measurement uncertainty
  float estimationVariance; // p, estimation uncertainty
  float modelVariance; // q, model uncertainty
  float currentEstimation;
  float previousEstimation;
  float kalmanGain = 0;

};

#endif