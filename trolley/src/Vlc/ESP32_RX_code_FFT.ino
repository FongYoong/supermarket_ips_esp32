/*
This program is to perform FFT on the ADC samples, and perform the distance estimation calculation based on the magnitude. 
Ultimately, the coordinate of an receiver will be estimated through trilateration algorithm.
*/

#include "arduinoFFT.h"

//RSS for 4 LEDs
double Pr_1;
double Pr_2;
double Pr_3;
double Pr_4;

//Enter frequency of the interested signal to view its RSS
double F_signal; // in (Hz)

//Diagonal distance from an LED to RX
double Dxyz1;
double Dxyz2;
double Dxyz3;
double Dxyz4;

//Estimated coordinate of RX
double RX_x;
double RX_y;


arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL 34 //for ESP32

const uint16_t samples = 2048; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 4096; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03


void setup()
{
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Ready");
  Serial.println("Please enter signal frequency in Hz");
  while (Serial.available() == 0) {
  }
  F_signal = Serial.parseFloat();
  Serial.print("Frequency Entered: ");
  Serial.println(F_signal);
}

void loop()
{
  /*SAMPLING*/
  microseconds = micros();
  for(int i=0; i<samples; i++)
  {
      vReal[i] = analogRead(CHANNEL)/4095.0*3.3; /*ESP32*/
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){
        //empty loop
      }
      microseconds += sampling_period_us;
  }

  /* window functions:
FFT_WIN_TYP_RECTANGLE
FFT_WIN_TYP_HAMMING
FFT_WIN_TYP_HANN
FFT_WIN_TYP_TRIANGLE
FFT_WIN_TYP_NUTTALL
FFT_WIN_TYP_BLACKMAN
FFT_WIN_TYP_BLACKMAN_NUTTALL
FFT_WIN_TYP_BLACKMAN_HARRIS
FFT_WIN_TYP_FLT_TOP (POOR frequency resolution, GOOD Spectral leakage, BEST amplitude accuracy)
FFT_WIN_TYP_WELCH
  */
  
  /* Print the results of the sampling according to time */
  Serial.println("ADC Data: {min, max}");
  //PrintVector(vReal, samples, SCL_TIME);
  PrintMaxMin(vReal,samples);
  
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_FLT_TOP, FFT_FORWARD);  /* Weigh data */
  //Serial.println("Weighed data:");
  //PrintVector(vReal, samples, SCL_TIME);
  
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  //Serial.println("Computed Real values:");
  //PrintVector(vReal, samples, SCL_INDEX);
  //Serial.println("Computed Imaginary values:");
  //PrintVector(vImag, samples, SCL_INDEX);
  
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  //PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
  //PrintMaxMin(vReal,samples);

  double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
  Serial.print("Major Peak (Hz):");
  Serial.println(x, 6); //Print out what frequency is the most dominant.

  //Obtain magnitude for 4 frequency component
  GetRSS(vReal, samples, F_signal,&Pr_1,&Pr_2,&Pr_3,&Pr_4);
  
  //Estimate distance based on the RSS
  DistEst(Pr_1,Pr_2,Pr_3,Pr_4,&Dxyz1,&Dxyz2,&Dxyz3,&Dxyz4);
  
  //Estimate coordinate based on 4 distance estimated
  CoorEst(Dxyz1,Dxyz2,Dxyz3,Dxyz4,&RX_x,&RX_y);
  
  //while(1); /* Run Once */
  delay(5000); /* Repeat after delay */
}


void PrintMaxMin(double *vData, uint16_t bufferSize){
  double MaxVal = vData[0];
  double MinVal = vData[0];
  double temp = 0;

  for (uint16_t i = 1; i < bufferSize; i++)
  {
      if(vData[i]>MaxVal)
      {
        MaxVal=vData[i];
        }
       else if (vData[i]<MinVal)
       {
        MinVal=vData[i];
        }
  }
  /*
  Serial.print("Min value: ");
  Serial.println(MinVal,6);
  Serial.print("Max value: ");
  Serial.println(MaxVal,6);
  Serial.println(" ");
  */
  Serial.print("{");
  Serial.print(MinVal,6);
  Serial.print(", ");
  Serial.print(MaxVal,6);
  Serial.println("}");
}
  
void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType){
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
        abscissa = ((i * 1.0) / samplingFrequency);
  break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
  break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

/*Calculate Perceived frequency for frequencies above sampling frequency*/
double GetAliasedFreq(double freq, double samplingfrequency){

double ratio = freq/samplingfrequency;
double AFreq = 0;
  if(ratio>1){
    ratio = freq/samplingfrequency - int(freq/samplingfrequency);
  }

  if(ratio>0.5){
    AFreq = (1-ratio)*samplingfrequency;
  }
  else{
    AFreq = ratio*samplingfrequency;
  }

  return AFreq;
}

void GetRSS(double *vData, uint16_t bufferSize, double F_signal, double *RSS1, double *RSS2, double *RSS3, double *RSS4){
//Please take note on this function
//Choose values for variables samples and samplingFrequency carefully
  /*
  double F1 = 1000;
  double F2 = 1250;
  double F3 = 1500;
  double F4 = 1750;
*/
//Harmonics
/*
if (2*F1<samplingFrequency)
{
  if(2*F1<samplingFrequency/2)
  {double F1har2 = 2*F1;}
  else
  {double F1har2 = samplingFrequency - 2*F1;}
}
else if (2*F1>=samplingFrequency && 2*F1)
{
  double F1har2 = 
  }
 */
 /*Harmonics component for: based on fs of 4096 Hz
  * 1kHz: [2]:2kHz, [3]1096Hz (3kHz), [4]96 Hz (4 kHz)
  * 1.25kHz: [2]1596Hz (2.5kHz), [3]346Hz (3.75kHz), [4]904Hz(5kHz)
  * 1.5kHz: [2]1096Hz (3kHz), [3]404Hz (4.5kHz), [4]1904Hz (6kHz)
  * 1.75kHz: [2]596Hz (3.5kHz), [3]1154Hz (5.25kHz), [4]1192Hz (7kHz)
  *no longer required to manually change this as it's automatically calculated in GetAliasedFreq() function
  */
  double F_0 = GetAliasedFreq(F_signal,samplingFrequency);
  double Fhar2 = GetAliasedFreq(2*F_signal,samplingFrequency); //2nd harmonic of fundamental frequency
  double Fhar3 = GetAliasedFreq(3*F_signal,samplingFrequency);
  double Fhar4 = GetAliasedFreq(4*F_signal,samplingFrequency);

  
  Serial.println(" ");
  Serial.println("RSS values obtained:");
  Serial.print("DC component(0 Hz): ");
  Serial.println(vData[0],6);
  Serial.print(F_0);
  Serial.print(" Hz: ");
  Serial.println(vData[int(F_0*samples/samplingFrequency)],6);
  Serial.print(Fhar2);
  Serial.print(" Hz: ");
  Serial.println(vData[int(Fhar2*samples/samplingFrequency)],6);
  Serial.print(Fhar3);
  Serial.print(" Hz: ");
  Serial.println(vData[int(Fhar3*samples/samplingFrequency)],6);
  Serial.print(Fhar4);
  Serial.print(" Hz: ");
  Serial.println(vData[int(Fhar4*samples/samplingFrequency)],6);
  Serial.println(" ");
  /*
  Serial.print(F_0);Serial.println(" Hz Harmonics:");
  Serial.print(Fhar2);Serial.print(" Hz: ");Serial.println(vData[int(Fhar2*samples/samplingFrequency)],6);
  Serial.print(Fhar3);Serial.print(" Hz: ");Serial.println(vData[int(Fhar3*samples/samplingFrequency)],6);
  Serial.print(Fhar4);Serial.print(" Hz: ");Serial.println(vData[int(Fhar4*samples/samplingFrequency)],6);
  */

  double ratio = 575.2960103; //Applicable for Flat-top window only
  Serial.println("Processed RSS data:");
  *RSS1 = vData[int(F_0*samples/samplingFrequency)]/(ratio);
  Serial.print(F_0);
  Serial.print(" Hz: ");
  Serial.println(*RSS1,6);
  *RSS2 = vData[int(Fhar2*samples/samplingFrequency)]/(ratio);
  Serial.print(Fhar2);
  Serial.print(" Hz: ");
  Serial.println(*RSS2,6);
  *RSS3 = vData[int(Fhar3*samples/samplingFrequency)]/(ratio);
  Serial.print(Fhar3);
  Serial.print(" Hz: ");
  Serial.println(*RSS3,6);
  *RSS4 = vData[int(Fhar4*samples/samplingFrequency)]/(ratio); 
  Serial.print(Fhar4);
  Serial.print(" Hz: ");
  Serial.println(*RSS4,6);
  Serial.println(" ");
  
}

/*RSS Distance Estimation*/
void DistEst(double RSS1, double RSS2, double RSS3, double RSS4, double *D1, double *D2, double *D3, double *D4){
  /*Distance Estimation variables*/
  //double height = 3; //3 meter
  double PD_Area = 0.0000052441; //5.2441mm2
  double m = 1; //TX light source lambertian pattern directionality 
  // m = 1 for half-power angle (hpa) of 60 for the LEDs
  // m =19.99372736 for hpa of 15 for normal small LEDs 
  
  //double M = 0.8047816999; //RX directionality 
  // M = 0.5128 for half-power angle of 65 for OPT101

  double Duty_cycle = 12/15;
  //double P_TX = 250.0/(0.45*1000000*samples); //transmitted power
  //double P_TX = 0.1/(0.45*1000000); //transmitted power  
  //double P_TX = 2000; //transmitted power
  double P_TX = 3.3*(1-Duty_cycle); //transmitted power updated on (14/6/2022)

  //Convert RSS voltage to watt 
  /*
  double P_RSS1 = RSS1/(0.45*1000000);
  double P_RSS2 = RSS2/(0.45*1000000);
  double P_RSS3 = RSS3/(0.45*1000000);
  double P_RSS4 = RSS4/(0.45*1000000);
  */
  
  //Lambertian DC channel gain formula
  /*
  *D1 = pow((PD_Area*pow(height,m+M)*P_TX)/(PI*P_RSS1),1/(2+m+M));
  *D2 = pow((PD_Area*pow(height,m+M)*P_TX)/(PI*P_RSS2),1/(2+m+M));
  *D3 = pow((PD_Area*pow(height,m+M)*P_TX)/(PI*P_RSS3),1/(2+m+M));
  *D4 = pow((PD_Area*pow(height,m+M)*P_TX)/(PI*P_RSS4),1/(2+m+M));
  */
  //Horizontal Lambertian gain formula
  *D1 = sqrt((m+1)*PD_Area*P_TX/(2.0*PI*RSS1));
  *D2 = sqrt((m+1)*PD_Area*P_TX/(2.0*PI*RSS2));
  *D3 = sqrt((m+1)*PD_Area*P_TX/(2.0*PI*RSS3));
  *D4 = sqrt((m+1)*PD_Area*P_TX/(2.0*PI*RSS4));

  Serial.println("Distance estimated: ");
  Serial.print("Dist1: ");
  Serial.print(*D1,6);Serial.println(" m");
  Serial.print("Dist2: ");
  Serial.print(*D2,6);Serial.println(" m");
  Serial.print("Dist3: ");
  Serial.print(*D3,6);Serial.println(" m");
  Serial.print("Dist4: ");
  Serial.print(*D4,6);Serial.println(" m");
  Serial.println(" ");
}

/*Trilateration Algorithm*/
void CoorEst(double Dist_xyz1, double Dist_xyz2, double Dist_xyz3, double Dist_xyz4, double *Xe, double *Ye ){
  /*Coordinate Estimation variables*/
  double LED1_x = 1; //LED 1 coordinate x
  double LED1_y = 1; //LED 1 coordinate y
  
  double LED2_x = 3;
  double LED2_y = 1;
  
  double LED3_x = 1;
  double LED3_y = 3;
  
  double LED4_x = 3;
  double LED4_y = 3;  

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

//  Serial.println("LED position:");
//  Serial.print("LED 1:    (");Serial.print(LED1_x,4);Serial.print(", ");Serial.print(LED1_y,4);Serial.println(")");
//  Serial.print("LED 2:    (");Serial.print(LED2_x,4);Serial.print(", ");Serial.print(LED2_y,4);Serial.println(")");
//  Serial.print("LED 3:    (");Serial.print(LED3_x,4);Serial.print(", ");Serial.print(LED3_y,4);Serial.println(")");
//  Serial.print("LED 4:    (");Serial.print(LED4_x,4);Serial.print(", ");Serial.print(LED4_y,4);Serial.println(")");
//  Serial.println(" ");
//  Serial.println("Estimated Receiver Coordinate:");
//  Serial.print("X:");Serial.print(*Xe,6);
//  Serial.print("    Y:");Serial.print(*Ye,6);
//  Serial.println(" ");
}
