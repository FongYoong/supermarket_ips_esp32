const int ledPin1 = 16;  // 16 corresponds to GPIO16
const int DutyCycle1 = 127; // 127/255 = 0.5,204/255=0.8
const double freq1 = 20;
const int ledChannel1 = 0;
const int resolution= 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel1, freq1, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin1, ledChannel1);
  Serial.begin(115200);
//  pinMode(16, OUTPUT);
}
 
void loop(){
  ledcWrite(ledChannel1, DutyCycle1);
  Serial.println(123);
//  digitalWrite(16, HIGH);
//  delay(500);
//  digitalWrite(16, LOW);
//  delay(500);
}
