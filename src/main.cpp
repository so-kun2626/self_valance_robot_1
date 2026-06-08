#include <Arduino.h>

const int motorPIN1 = 13;
const int motorPIN2 = 27;
const int motorPIN3 = 25;
const int motorPIN4 = 26;

void setup() {
  // put your setup code here, to run once:
 pinMode(motorPIN1, OUTPUT);
 pinMode(motorPIN2, OUTPUT);
 pinMode(motorPIN3, OUTPUT);
 pinMode(motorPIN4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(motorPIN1,HIGH);
  digitalWrite(motorPIN2,LOW);
  digitalWrite(motorPIN3,HIGH);
  digitalWrite(motorPIN4,LOW);
  delay(1000);
}




