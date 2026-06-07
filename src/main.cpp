#include <Arduino.h>

const int motorPIN1 = 13;
const int motorPIN2 = 27;
const int motorPIN3 = 26;
const int motorPIN4 = 25;

void setup() {
  // put your setup code here, to run once:
 pinMode(motorPIN1, OUTPUT);
 pinMode(motorPIN2, OUTPUT);
 pinMode(motorPIN3, OUTPUT);
 pinMode(motorPIN4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int a=0; a<255; a+=20) {
    analogWrite(motorPIN1, a);
    analogWrite(motorPIN2, 0);
    delay(100);
  }
  for (int a=0; a<255; a+=20) {
    analogWrite(motorPIN1, 0);
    analogWrite(motorPIN2, a);
    delay(100);
  }
}




