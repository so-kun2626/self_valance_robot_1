#include<Arduino.h>

const int motorPin1 = 13;
const int motorPin2 = 27;
const int motorPin3 = 25;
const int motorPin4 = 26;

const int pwmCH1 = 0;
const int pwmCH2 = 1;
const int pwmCH3 = 2;
const int pwmCH4 = 3;

const int pwmFreq = 20000; // 20kHz
const int pwmResolution = 10; // 10ビット分解能

void setup(){
    Serial.begin(115200);

    ledcSetup(pwmCH1, pwmFreq, pwmResolution);
    ledcSetup(pwmCH2, pwmFreq, pwmResolution);
    ledcSetup(pwmCH3, pwmFreq, pwmResolution);
    ledcSetup(pwmCH4, pwmFreq, pwmResolution);

    ledcAttachPin(motorPin1, pwmCH1);
    ledcAttachPin(motorPin2, pwmCH2);
    ledcAttachPin(motorPin3, pwmCH3);
    ledcAttachPin(motorPin4, pwmCH4);

    Serial.println("PWM Test start!");
    delay(2000);
}

void loop(){

    int power = 1000; // PWMの出力値（0〜1023）

    Serial.println("Forward");
    ledcWrite(pwmCH1, power);
    ledcWrite(pwmCH2, 0);
    ledcWrite(pwmCH3, power);
    ledcWrite(pwmCH4, 0);

    delay(2000);

    Serial.println("Stop");
    ledcWrite(pwmCH1, 0);
    ledcWrite(pwmCH2, 0);
    ledcWrite(pwmCH3, 0);
    ledcWrite(pwmCH4, 0);

    delay(2000);
    
    Serial.println("Backward");
    ledcWrite(pwmCH1, 0);
    ledcWrite(pwmCH2, power);
    ledcWrite(pwmCH3, 0);
    ledcWrite(pwmCH4, power);

    delay(2000);

    Serial.println("Stop");
    ledcWrite(pwmCH1, 0);
    ledcWrite(pwmCH2, 0);
    ledcWrite(pwmCH3, 0);
    ledcWrite(pwmCH4, 0);

    delay(2000);
}