#include "KalmanFilter1D.h"
#include <math.h>

KalmanFilter1D::KalmanFilter1D(float initialEstimation, float measurementVariance, float estimationVariance, float modelVariance)
                                : currentEstimation{initialEstimation}, previousEstimation{initialEstimation},
                                measurementVariance{measurementVariance}, estimationVariance{estimationVariance}, modelVariance{modelVariance}
{

}

float KalmanFilter1D::updateEstimate(float measurement)
{
  kalmanGain = estimationVariance/(estimationVariance + measurementVariance);
  currentEstimation = previousEstimation + kalmanGain * (measurement - previousEstimation);
  estimationVariance =  (1.0 - kalmanGain) * estimationVariance + fabs(previousEstimation - currentEstimation) * modelVariance;
  previousEstimation = currentEstimation;
  return currentEstimation;
}