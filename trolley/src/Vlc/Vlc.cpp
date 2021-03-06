#include "Vlc.h"

Utilities::RepeatingTask Vlc::photodiodeTask(PHOTODIODE_INTERVAL);

arduinoFFT Vlc::fft = arduinoFFT();
unsigned int Vlc::sampling_period_us = round(1000000*(1.0/FFT_SAMPLING_FREQUENCY));
unsigned long Vlc::microseconds = 0;

double Vlc::Pr_1 = 0;
double Vlc::Pr_2 = 0;
double Vlc::Pr_3 = 0;
double Vlc::Pr_4 = 0;
double Vlc::Dxyz1 = 0;
double Vlc::Dxyz2 = 0;
double Vlc::Dxyz3 = 0;
double Vlc::Dxyz4 = 0;
double Vlc::RX_x = 0;
double Vlc::RX_y = 0;

double Vlc::vReal[FFT_SAMPLES];
double Vlc::vImag[FFT_SAMPLES];

void Vlc::setup()
{

}

void Vlc::run()
{
    if (photodiodeTask.isReady())
    {
        double photodiodeValue = analogReadMilliVolts(PIN_PHOTODIODE);
        Serial.println(String("Photodiode (mV):") + photodiodeValue);
        microseconds = micros();
        //Serial.println(String("Sampling period: ") + sampling_period_us);
        for(int i=0; i<FFT_SAMPLES; i++)
        {
            vReal[i] = analogRead(PIN_PHOTODIODE)/4095.0*3.3; /*ESP32*/
            vImag[i] = 0;
            while(micros() - microseconds < sampling_period_us){
                //empty loop
            }
            microseconds += sampling_period_us;
        }
        /* Print the results of the sampling according to time */
        // Serial.println("Data:");
        // PrintVector(vReal, FFT_SAMPLES, SCL_TIME);
        
        fft.Windowing(vReal, FFT_SAMPLES, FFT_WIN_TYP_BLACKMAN, FFT_FORWARD);  /* Weigh data */
        //Serial.println("Weighed data:");
        //PrintVector(vReal, samples, SCL_TIME);
        
        fft.Compute(vReal, vImag, FFT_SAMPLES, FFT_FORWARD); /* Compute FFT */
        //Serial.println("Computed Real values:");
        //PrintVector(vReal, FFT_SAMPLES, SCL_INDEX);
        //Serial.println("Computed Imaginary values:");
        //PrintVector(vImag, FFT_SAMPLES, SCL_INDEX);
        
        fft.ComplexToMagnitude(vReal, vImag, FFT_SAMPLES); /* Compute magnitudes */
        // Serial.println("Computed magnitudes:");
        // PrintVector(vReal, (FFT_SAMPLES >> 1), SCL_FREQUENCY);

        
        double x = fft.MajorPeak(vReal, FFT_SAMPLES, FFT_SAMPLING_FREQUENCY);
        Serial.println("Major Peak (Hz):");
        Serial.print(x, 6); //Print out what frequency is the most dominant.
        Serial.println("");
        
        //Obtain magnitude for 4 frequency component
        EstimateRSS(vReal, FFT_SAMPLES, &Pr_1,&Pr_2,&Pr_3,&Pr_4);
        
        //Estimate distance based on the RSS
        EstimateDistance(Pr_1,Pr_2,Pr_3,Pr_4,&Dxyz1,&Dxyz2,&Dxyz3,&Dxyz4);
        
        //Estimate coordinate based on 4 distance estimated
        EstimateCoordinate(Dxyz1,Dxyz2,Dxyz3,Dxyz4,&RX_x,&RX_y);
    }
}

void Vlc::PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
  break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / FFT_SAMPLING_FREQUENCY);
  break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * FFT_SAMPLING_FREQUENCY) / FFT_SAMPLES);
  break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY) {
      Serial.print("Hz");
    }
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

void Vlc::EstimateRSS(double *vData, uint16_t bufferSize, double *RSS1, double *RSS2, double *RSS3, double *RSS4)
{
    //Choose values for variables samples and samplingFrequency carefully
    double F1 = 1000;
    double F2 = 1250;
    double F3 = 1500;
    double F4 = 1750;

    double aTHD = 1; //scale factor for tuning range: 0 - 1

    double NT = FFT_SAMPLES/FFT_SAMPLING_FREQUENCY;

    *RSS1 = vData[int(F1*NT)]/(aTHD*FFT_SAMPLES);
    Serial.print(F1);
    Serial.print(" Hz: ");
    Serial.println(*RSS1,6);
    *RSS2 = vData[int(F2*NT)]/(aTHD*FFT_SAMPLES);
    Serial.print(F2);
    Serial.print(" Hz: ");
    Serial.println(*RSS2,6);
    *RSS3 = vData[int(F3*NT)]/(aTHD*FFT_SAMPLES);
    Serial.print(F3);
    Serial.print(" Hz: ");
    Serial.println(*RSS3,6);
    *RSS4 = vData[int(F4*NT)]/(aTHD*FFT_SAMPLES); 
    Serial.print(F4);
    Serial.print(" Hz: ");
    Serial.println(*RSS4,6);
}

/*RSS Distance Estimation*/
void Vlc::EstimateDistance(
    double RSS1, double RSS2, double RSS3, double RSS4, 
    double *D1, double *D2, double *D3, double *D4) {

  /*Distance Estimation variables*/
  double height = 1; // 1 meter
  double PD_Area = 0.0000052441; //5.2441 mm2
  double m = 1; //TX light source lambertian pattern directionality 
  // m = 1 for half-power angle (hpa) of 60 for the LEDs
  // m =19.99372736 for hpa of 15 for normal small LEDs 
  double M = 0.8047816999; //RX directionality
  // M = 0.5128 for half-power angle of 65 for OPT101
  double P_TX = 2.6; //transmitted power
  //double P_TX = 0.1/(0.45*1000000); //transmitted power
  
  //Convert RSS voltage to watt 
  double P_RSS1 = RSS1;
  double P_RSS2 = RSS2;
  double P_RSS3 = RSS3;
  double P_RSS4 = RSS4;

  //Lambertian DC channel gain formula
  *D1 = pow((PD_Area*P_TX*pow(height,m+M))/(PI*P_RSS1), 1/(2+m+M));
  *D2 = pow((PD_Area*P_TX*pow(height,m+M))/(PI*P_RSS2), 1/(2+m+M));
  *D3 = pow((PD_Area*P_TX*pow(height,m+M))/(PI*P_RSS3), 1/(2+m+M));
  *D4 = pow((PD_Area*P_TX*pow(height,m+M))/(PI*P_RSS4), 1/(2+m+M));

  // Serial.print("Dist1: ");
  // Serial.println(*D1,6);
  // Serial.print("Dist2: ");
  // Serial.println(*D2,6);
  // Serial.print("Dist3: ");
  // Serial.println(*D3,6);
  // Serial.print("Dist4: ");
  // Serial.println(*D4,6);
}

/*Multilateration Algorithm*/
void Vlc::EstimateCoordinate(
    double Dist_xyz1, double Dist_xyz2, 
    double Dist_xyz3, double Dist_xyz4, double *Xe, double *Ye ) {
  /*Coordinate Estimation variables*/
  double LED1_x = 1.5; //LED 1 coordinate x
  double LED1_y = 0; //LED 1 coordinate y
  
  double LED2_x = 1.5;
  double LED2_y = 1;
  
  double LED3_x = 0;
  double LED3_y = 0;
  
  double LED4_x = 0;
  double LED4_y = 1;

  //Elements of Matrix A (A) [3x2]
  double a11=LED2_x - LED1_x; double a12 = LED2_y - LED1_y;
  double a21=LED3_x - LED1_x; double a22 = LED3_y - LED1_y;
  double a31=LED4_x - LED1_x; double a32 = LED4_y - LED1_y;

  //Elements of Transpose Matrix A (A^T) [2x3]
  double aT11=a11; double aT12=a21; double aT13=a31;
  double aT21=a12; double aT22=a22; double aT23=a32;

  //Elements of (A^T*A) [2x2]
  double aTa11 = sq(a11)+sq(a21)+sq(a31); double aTa12 = a11*a12+a21*a22+a31*a32;
  double aTa21 = a12*a11+a22*a21+a32*a31; double aTa22 = sq(a12)+sq(a22)+sq(a32);

  //Elements of (A^T*A)^-1 [2x2]
  double det_aTa = 1/(aTa22*aTa11 - aTa12*aTa21);
  double aTa_Inv11 = det_aTa*aTa22; double aTa_Inv12 = det_aTa*(-aTa12);
  double aTa_Inv21 = det_aTa*(-aTa21); double aTa_Inv22 = det_aTa*(aTa11);

  //Elements of (A^T*A)^-1*A^T [2x3]
  double aTa_Inv_aT11 = aTa_Inv11*aT11+aTa_Inv12*aT21; double aTa_Inv_aT12 = aTa_Inv11*aT12+aTa_Inv12*aT22; double aTa_Inv_aT13 = aTa_Inv11*aT13+aTa_Inv12*aT23;
  double aTa_Inv_aT21 = aTa_Inv21*aT11+aTa_Inv22*aT21; double aTa_Inv_aT22 = aTa_Inv21*aT12+aTa_Inv22*aT22; double aTa_Inv_aT23 = aTa_Inv21*aT13+aTa_Inv22*aT23;

  //Elements of matrix B (B) [3x1]
  double b11 = (sq(Dist_xyz1)-sq(Dist_xyz2)+sq(LED2_x)+sq(LED2_y)-sq(LED1_x)-sq(LED1_y))/2;
  double b21 = (sq(Dist_xyz1)-sq(Dist_xyz3)+sq(LED3_x)+sq(LED3_y)-sq(LED1_x)-sq(LED1_y))/2;
  double b31 = (sq(Dist_xyz1)-sq(Dist_xyz4)+sq(LED4_x)+sq(LED4_y)-sq(LED1_x)-sq(LED1_y))/2;

  //Elements of (A^T*A)^-1*A^T*B [2x1]
  *Xe = aTa_Inv_aT11*b11 + aTa_Inv_aT12*b21 + aTa_Inv_aT13*b31;
  *Ye = aTa_Inv_aT21*b11 + aTa_Inv_aT22*b21 + aTa_Inv_aT23*b31;

//   Serial.println("LED position:");
//   Serial.print("LED 1:    (");Serial.print(LED1_x,4);Serial.print(", ");Serial.print(LED1_y,4);Serial.println(")");
//   Serial.print("LED 2:    (");Serial.print(LED2_x,4);Serial.print(", ");Serial.print(LED2_y,4);Serial.println(")");
//   Serial.print("LED 3:    (");Serial.print(LED3_x,4);Serial.print(", ");Serial.print(LED3_y,4);Serial.println(")");
//   Serial.print("LED 4:    (");Serial.print(LED4_x,4);Serial.print(", ");Serial.print(LED4_y,4);Serial.println(")");
  Serial.println("Estimated Receiver Coordinate:");
  Serial.print("(X: ");
  Serial.print(*Xe,6);
  Serial.print(", Y: ");
  Serial.print(*Ye,6);
  Serial.print(")");
  Serial.println("");
}
