const int ledPin1 = 16;  // 16 corresponds to GPIO16 (red)
const int ledPin2 = 17;  // 17 corresponds to GPIO17 (yellow)
const int ledPin3 = 19;  // 19 corresponds to GPIO1 (blue)
const int ledPin4 = 18;  // 18 corresponds to GPIO3 (black)

const int dutyCycle = 127; // 127/255 = 0.5,204/255=0.8

const double freq1 = 1000;
const double freq2 = 1250;
const double freq3 = 1500;
const double freq4 = 1750;

const int ledChannel1 = 2;
const int ledChannel2 = 4;
const int ledChannel3 = 6;
const int ledChannel4 = 8;

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
  ledcWrite(ledChannel1, dutyCycle);
  ledcWrite(ledChannel2, dutyCycle);
  ledcWrite(ledChannel3, dutyCycle);
  ledcWrite(ledChannel4, dutyCycle);
  Serial.begin(115200);
}
 
void loop(){
  Serial.println("Modulating lights...");
  delay(1000);
//  digitalWrite(16, HIGH);
//  digitalWrite(16, LOW);
//  delay(500);
}
