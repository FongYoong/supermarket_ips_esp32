const int ledPin1 = 16;  // 16 corresponds to GPIO16 (red)
const int ledPin2 = 17;  // 17 corresponds to GPIO17 (yellow)
const int ledPin3 = 19;  // 1 corresponds to GPIO1 (blue)
const int ledPin4 = 18;  // 3 corresponds to GPIO3 (black)

const int DutyCycle1 = 127; // 127/255 = 0.5,204/255=0.8

const double freq1 = 1000;
const double freq2 = 1250;
const double freq3 = 1500;
const double freq4 = 1750;

const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;
const int ledChannel4 = 3;

const int resolution= 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel1, freq1, resolution);
  ledcSetup(ledChannel2, freq2, resolution);
  ledcSetup(ledChannel3, freq3, resolution);
  ledcSetup(ledChannel4, freq4, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin1, ledChannel1); 
  ledcAttachPin(ledPin2, ledChannel2);
  ledcAttachPin(ledPin3, ledChannel3);
  ledcAttachPin(ledPin4, ledChannel4);
  
  Serial.begin(115200);
//  pinMode(16, OUTPUT);
}
 
void loop(){
  ledcWrite(ledChannel1, DutyCycle1);
  ledcWrite(ledChannel2, DutyCycle1);
  ledcWrite(ledChannel3, DutyCycle1);
  ledcWrite(ledChannel4, DutyCycle1);
  
  Serial.println(123);
//  digitalWrite(16, HIGH);
//  delay(500);
//  digitalWrite(16, LOW);
//  delay(500);
}
