#ifndef VLC_H
#define VLC_H

#include "Arduino.h"
#include "arduinoFFT.h"
#include "../Utilities.h"
#include "../Pins/Pins.h"
#include "KalmanFilter1D.h"

#define PHOTODIODE_INTERVAL 50
#define FFT_SAMPLES 2048
#define FFT_SAMPLING_FREQUENCY 4096.0
// uint16_t Vlc::samples = 2048; //This value MUST ALWAYS be a power of 2
// double Vlc::samplingFrequency = 4096; //Hz, must be less than 10000 due to ADC

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

class Vlc
{
public:
    static void setup();
    static void run();

    static Utilities::RepeatingTask photodiodeTask;

    static arduinoFFT fft;
    static unsigned int sampling_period_us;
    static unsigned long microseconds;
    
    // These are the input and output vectors. Input vectors receive computed results from FFT
    static double vReal[];
    static double vImag[];

    //RSS for 4 LEDs
    static double Pr_1;
    static double Pr_2;
    static double Pr_3;
    static double Pr_4;
    //Diagonal distance from an LED to RX
    static double Dxyz1;
    static double Dxyz2;
    static double Dxyz3;
    static double Dxyz4;
    //Estimated coordinate of RX
    static double RX_x;
    static double RX_y;

    // Kalman filter
    static KalmanFilter1D rss1KalmanFilter;
    static KalmanFilter1D rss2KalmanFilter;
    static KalmanFilter1D rss3KalmanFilter;
    static KalmanFilter1D rss4KalmanFilter;
    // static KalmanFilter1D xKalmanFilter;
    // static KalmanFilter1D yKalmanFilter;

    static void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType);
    static void EstimateRSS(double *vData, uint16_t bufferSize, double *RSS1, double *RSS2, double *RSS3, double *RSS4);
    static double RemoveRSSNoise(double RSS);
    static void EstimateDistance(double RSS1, double RSS2, double RSS3, double RSS4, double *D1, double *D2, double *D3, double *D4);
    static void EstimateCoordinate(double Dist_xyz1, double Dist_xyz2, double Dist_xyz3, double Dist_xyz4, double *Xe, double *Ye );
    static void EstimateKalman();
};

#endif