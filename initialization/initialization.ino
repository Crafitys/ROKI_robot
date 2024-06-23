// This code is only used for initialize your robot at first start. When it started on your robot please put all limbs in position in accordance with the instructions
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver myServo = Adafruit_PWMServoDriver();

#define SERVOMIN 500
#define SERVOMAX 2500

void setup() {
  Serial.begin(9600);
  
  myServo.begin();
  myServo.setPWMFreq(60);
  delay(500);
}

void loop() 
{
  for (int i = 0; i < 12; i++) {
    myServo.writeMicroseconds(i,mapf(90));
  
  }
  
}

int mapf(float angle){
  int result;
  result = (angle - 0) * (2500 - 500) / (180 - 0) + 500;
  return result;
  }