#include <EEPROM.h>
// define the number of by
#define EEPROM_SIZE 6
int duty1=0;
int duty2=0;
int duty3=0;
int flag1=0;
int flag2=0;
int flag3=0;
const int ledPin = 16; // 16 corresponds to GPIO16
const int ledPin2 = 17; // 17 corresponds to GPIO17
const int ledPin3 = 5; // 5 corresponds to GPIO5
const int button =4;
int buttonState =0;
int sign=0;
// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
const int freq2 = 5000;
const int ledChannel2 = 1;
const int resolution2 = 8;
const int freq3 = 5000;
const int ledChannel3 = 2;
const int resolution3 = 8;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 150;    // the debounce time; increase if the output flickers
unsigned long pausestate=0;
int lastButtonState = HIGH;
int insu=0;
void IRAM_ATTR detectButt() {

if(insu==1)return;
if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    Serial.println(pausestate);
      pausestate^=1;
      Serial.println("Interrupt occurs");
    // if the button state has changed:
}
lastDebounceTime = millis();
    
  

}

void setup() {
  Serial.begin(115200);
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
// configure LED PWM functionalitites
ledcSetup(ledChannel, freq, resolution);
ledcSetup(ledChannel2, freq2, resolution2);
ledcSetup(ledChannel3, freq3, resolution3);
// attach the channel to the GPIOs
ledcAttachPin(ledPin, ledChannel);
ledcAttachPin(ledPin2, ledChannel2);
ledcAttachPin(ledPin3, ledChannel3);
pinMode(button,INPUT);
attachInterrupt(digitalPinToInterrupt(button), detectButt, RISING);

duty1 = EEPROM.read(0);
duty2 = EEPROM.read(1);
duty3 = EEPROM.read(2);
flag1 = EEPROM.read(3);
flag2 = EEPROM.read(4);
flag3 = EEPROM.read(5);
 Serial.print("처음 불러들인값");
for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(EEPROM.read(i)); Serial.print(" ");
  }
  ledcWrite(ledChannel, duty1);
ledcWrite(ledChannel2, duty2);
ledcWrite(ledChannel3, duty3);
}

void loop(){
// increase the LED brightnesskjjk
buttonState = digitalRead(button);
if(buttonState==0){
  sign++;
  delay(100);
}
if(sign>0){
for(int dutyCycle = 0; dutyCycle <= 510; dutyCycle=dutyCycle+1) {
// changing the LED brightness with PWM
buttonState = digitalRead(button);
/*
if (buttonState!= lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }*/
if(duty1>255){
  flag1=1;
  duty1=255;
}else if(duty1<0){
  flag1=0;
  duty1=0;
}if(duty2>255){
  flag2=1;
  duty2=255;
}else if(duty2<0){
  flag2=0;
    duty2=0;
  }if(duty3>255){
    flag3=1;
  duty3=255;
}else if(duty3<0){
  flag3=0;
  duty3=0;
}


if(flag1==0){
  duty1=duty1+3;
}else if(flag1==1){
  duty1=duty1-3;
}if(flag2==0){
  duty2=duty2+6;
}else if(flag2==1){
  duty2=duty2-6;
}if(flag3==0){
  duty3=duty3+10;
}else if(flag3==1){
  duty3=duty3-10;
}

if (pausestate){
EEPROM.write(0, duty1);
EEPROM.write(1, duty2);
EEPROM.write(2, duty3);
EEPROM.write(3, flag1);
EEPROM.write(4, flag2);
EEPROM.write(5, flag3);
EEPROM.commit();  
Serial.println("저장된 값");
for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(EEPROM.read(i)); Serial.print(" ");
  }
Serial.println(" ");
while(pausestate) {delay(5);}
}// wait breaking pause
else delay(5);

ledcWrite(ledChannel, duty1);
ledcWrite(ledChannel2, duty2);
ledcWrite(ledChannel3, duty3);
lastButtonState = buttonState;
delay(15);
}
}
}
