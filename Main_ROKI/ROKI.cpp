#include "ROKI.h"
#include <math.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
Adafruit_PWMServoDriver myServo = Adafruit_PWMServoDriver(0x40);

const float longF = 98.7;
const float longT = 119.5;
const float longE = 44;
int currentAngle[12];

float RadToDeg(double rad) {
  return float (rad * 180 / 3.1416);
}

float LawOfCosines(float a, float b, float c){ 
  return acos((a*a + b*b - c*c) / (2 * a * b));
}

float calcAngleF(float x,float z){
  float dist = sqrt(x*x + z*z);
  float A1 = RadToDeg(LawOfCosines(dist, longF, longT));
  float A2 = RadToDeg(LawOfCosines(x, dist, z));

  return (A1 + A2);
}

float calcAngleT(float x,float z) 
{
  float distance = sqrt(x*x + z*z);
  float A3 = RadToDeg(LawOfCosines(longT, longF, distance));
  return (A3);
}

float calcAngleE(float y, float z){
  if (y <= -44){
    float Hypo = sqrt(((y+longE)*(y+longE))+z*z);
    float Zj = sqrt(Hypo*Hypo - longE*longE);
    float AHypo = RadToDeg(acos(Zj/Hypo));
    float Hypo2 = RadToDeg(acos((z)/Hypo));
    return 90-(AHypo + Hypo2);
  }
  else {
    float Hypo = sqrt(((y+longE)*(y+longE))+z*z);
    float Zj = sqrt(Hypo*Hypo - longE*longE);
    float AHypo = RadToDeg(acos(Zj/Hypo));
    float AHypo2 = RadToDeg(acos(z/Hypo));
    return 90-(AHypo - AHypo2);
  }
}

float calcZposB(float y, float z){
  
  float Hypo = sqrt(((y+longE)*(y+longE))+z*z);
  float Zj = sqrt(Hypo*Hypo - longE*longE);
  return Zj;  
}

int mapf(float angle){
  int result;
  result = (angle - 0) * (2500 - 500) / (180 - 0) + 500;
  return result;
}
int imapf(float angle){
  int result;
  result = (angle - 500) * (180 - 0) / (2500 - 500) + 0;
  return result;
}

void moveServo(int topos0, int topos1, int topos2, int  topos3, int topos4, int topos8, int topos5, int topos9, int topos6, int topos10, int topos7, int topos11, int duration) {
  int decoupage = 10;
  int diff0 =((currentAngle[0] - topos0)/decoupage);
  int diff1 =((currentAngle[1] - topos1)/decoupage);
  int diff2 =((currentAngle[2] - topos2)/decoupage);
  int diff3 =((currentAngle[3] - topos3)/decoupage);
  int diff4 =((currentAngle[4] - topos4)/decoupage);
  int diff8 =((currentAngle[8] - topos8)/decoupage);
  int diff9 =((currentAngle[9] - topos9)/decoupage);
  int diff5 =((currentAngle[5] - topos5)/decoupage);
  int diff6 =((currentAngle[6] - topos6)/decoupage);
  int diff10 =((currentAngle[10] - topos10)/decoupage);
  int diff7 =((currentAngle[7] - topos7)/decoupage);
  int diff11 =((currentAngle[11] - topos11)/decoupage);
  
  for (int i = 1; i <= decoupage; i++) {
    myServo.writeMicroseconds(0,currentAngle[0]-diff0);
    currentAngle[0] = currentAngle[0]-diff0;
    myServo.writeMicroseconds(1,currentAngle[1]-diff1);
    currentAngle[1] = currentAngle[1]-diff1;
    myServo.writeMicroseconds(2,currentAngle[2]-diff2);
    currentAngle[2] = currentAngle[2]-diff2;
    myServo.writeMicroseconds(3,currentAngle[3]-diff3);
    currentAngle[3] = currentAngle[3]-diff3;
    myServo.writeMicroseconds(4,currentAngle[4]-diff4);
    currentAngle[4] = currentAngle[4]-diff4;
    myServo.writeMicroseconds(8,currentAngle[8]-diff8);
    currentAngle[8] = currentAngle[8]-diff8;
    myServo.writeMicroseconds(5,currentAngle[5]-diff5);
    currentAngle[5] = currentAngle[5]-diff5;
    myServo.writeMicroseconds(9,currentAngle[9]-diff9);
    currentAngle[9] = currentAngle[9]-diff9;
    myServo.writeMicroseconds(6,currentAngle[6]-diff6);
    currentAngle[6] = currentAngle[6]-diff6;
    myServo.writeMicroseconds(10,currentAngle[10]-diff10);
    currentAngle[10] = currentAngle[10]-diff10;
    myServo.writeMicroseconds(7,currentAngle[7]-diff7);
    currentAngle[7] = currentAngle[7]-diff7;
    myServo.writeMicroseconds(11,currentAngle[11]-diff11);
    currentAngle[11] = currentAngle[11]-diff11;
    delay(duration);
  }
}


void moveArm(float Xpos1, float Zpos1, float Ypos1, float Xpos2, float Zpos2, float Ypos2, float Xpos3, float Zpos3, float Ypos3, float Xpos4, float Zpos4, float Ypos4, int vitesse) {
    int topos0= mapf(180-calcAngleE(Ypos1,Zpos1));
    int topos1= mapf(180-calcAngleE(Ypos2,Zpos2));
    int topos2= mapf(calcAngleE(Ypos3,Zpos3));
    int topos3= mapf(calcAngleE(Ypos4,Zpos4));
    float Zpos1B = calcZposB(Ypos1,Zpos1);
    int topos4 = mapf(calcAngleF(Xpos1,Zpos1B));
    int topos8 = mapf(calcAngleT(Xpos1,Zpos1B));
    float Zpos2B = calcZposB(Ypos2,Zpos2);
    int topos5 = mapf(calcAngleF(Xpos2,Zpos2B));
    int topos9 = mapf(calcAngleT(Xpos2,Zpos2B));
    float Zpos3B = calcZposB(Ypos3,Zpos3);
    int topos6 = mapf(180-calcAngleF(Xpos3,Zpos3B));
    int topos10 = mapf(180-calcAngleT(Xpos3,Zpos3B));
    float Zpos4B = calcZposB(Ypos4,Zpos4);
    int topos7 = mapf(180-calcAngleF(Xpos4,Zpos4B));
    int topos11 = mapf(180-calcAngleT(Xpos4,Zpos4B));
    moveServo(topos0,topos1,topos2,topos3,topos4,topos8,topos5,topos9,topos6,topos10,topos7,topos11,vitesse); 
    server.handleClient(); 
}

void fastArm(float Xpos1,float Zpos1,float Xpos2, float Zpos2, float Xpos3, float Zpos3, float Xpos4, float Zpos4){
  myServo.writeMicroseconds(4,mapf(calcAngleF(Xpos1,Zpos1)));
  currentAngle[4]= mapf(calcAngleF(Xpos1,Zpos1));
  myServo.writeMicroseconds(8,mapf(calcAngleT(Xpos1,Zpos1)));
  currentAngle[8]= mapf(calcAngleF(Xpos1,Zpos1));
  myServo.writeMicroseconds(5,mapf(calcAngleF(Xpos2,Zpos2)));
  currentAngle[5]= mapf(calcAngleF(Xpos2,Zpos2));
  myServo.writeMicroseconds(9,mapf(calcAngleT(Xpos2,Zpos2)));
  currentAngle[9]= mapf(calcAngleF(Xpos2,Zpos2));
  myServo.writeMicroseconds(6,mapf(180-calcAngleF(Xpos3,Zpos3)));
  currentAngle[6]= mapf(180-calcAngleF(Xpos3,Zpos3));
  myServo.writeMicroseconds(10,mapf(180-calcAngleT(Xpos3,Zpos3)));
  currentAngle[10]= mapf(180-calcAngleF(Xpos3,Zpos3));
  myServo.writeMicroseconds(7,mapf(180-calcAngleF(Xpos4,Zpos4)));
  currentAngle[7]= mapf(180-calcAngleF(Xpos4,Zpos4));
  myServo.writeMicroseconds(11,mapf(180-calcAngleT(Xpos4,Zpos4)));
  currentAngle[11]= mapf(180-calcAngleF(Xpos4,Zpos4));
}

void marche(){
  moveArm(1,140,0,-20,160,0,-20,160,0,1,140,0,3);
  
  moveArm(60,140,0,-20,160,0,-20,160,0,60,140,0,1);
 
  moveArm(60,160,0,-20,160,0,-20,160,0,60,160,0,3);
 
  moveArm(1,160,0,-20,140,0,-20,140,0,1,160,0,3);
  
  moveArm(1,160,0,40,140,0,40,140,0,1,160,0,1);
  
  moveArm(1,160,0,40,160,0,40,160,0,1,160,0,3);
}

void cours(){
 
  moveArm(-30,180,0,30,160,0,30,160,0,-30,180,0,3);
  
  moveArm(-30,160,0,30,180,0,30,180,0,-30,160,0,3);
  
  moveArm(30,160,0,-30,180,0,-30,180,0,30,160,0,3);

  moveArm(30,180,0,-30,160,0,-30,160,0,30,180,0,3);
}

void gauche(){
  
  moveArm(1,140,35,1,160,0,1,160,0,1,140,35,2);
  delay(50);
  
  moveArm(-20,160,35,1,160,0,1,160,0,20,160,35,2);
  delay(50);
  
  moveArm(1,160,-35,1,130,0,1,130,0,1,160,-35,3);
  delay(50);
  
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,2);
  delay(50);
}

void droite(){ 
   moveArm(1,160,0,1,140,35,1,140,35,1,160,0,2);
  delay(50);
  
  moveArm(1,160,0,20,160,35,-20,160,35,1,160,0,2);
  delay(50);
  
  moveArm(1,130,0,1,160,-35,1,160,-35,1,130,0,3);
  delay(50);
  
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,2);
  delay(50);
   
}
void regardG(){
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,20);
  moveArm(1,140,-50,1,160,35,1,160,50,1,160,-35,50);
  delay(1000);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,50);
}
void regardD(){
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,20);
  moveArm(1,160,50,1,160,-35,1,140,-50,1,160,35,50);
  delay(1000);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,50);
}

void initialisation(){
  for (int i = 0; i < 12; i++) {
    myServo.writeMicroseconds(i,mapf(90));
    currentAngle[i] = mapf(90);
  }
  moveArm(1,180,0,1,180,0,1,180,0,1,180,0,70);
}

void attente(){
  moveArm(1,180,0,1,180,0,1,180,0,1,180,0,70);
}

void assis(){
  moveArm(1,200,0,50,100,0,1,200,0,50,100,0,70);
  delay(8000);
  moveArm(1,120,0,50,100,0,1,120,0,50,100,0,20);
  moveArm(1,120,0,50,150,0,1,120,0,50,150,0,10);
  
}

void vague(){
  moveArm(1,110,30,1,110,30,1,180,0,1,180,0,30);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,30);
  moveArm(1,180,0,1,180,0,1,110,30,1,110,30,30);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,30);
  moveArm(1,110,30,1,110,30,1,180,0,1,180,0,30);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,30);
  moveArm(1,180,0,1,180,0,1,110,30,1,110,30,30);
  moveArm(1,160,0,1,160,0,1,160,0,1,160,0,30);
}

void couche(){
  moveArm(50,100,0,50,100,0,50,100,0,50,100,0,50);
  delay(8000);
}

void testpos(){
  moveArm(1,160,100,1,160,0,1,160,0,1,160,0,75);
  
}

unsigned long measureDistance(int triggerPin, int echoPin) {
  // Envoi d'une impulsion de déclenchement
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);

  unsigned long distance = duration * 0.034 / 2;

  return distance;
}
