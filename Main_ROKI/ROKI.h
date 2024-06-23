#include <WiFi.h>
#include <WebServer.h>
extern WebServer server;

float RadToDeg(double rad);

float LawOfCosines(float a, float b, float c);

float calcAngleF(float x,float z);

float calcAngleT(float x,float z);

float calcAngleE(float y, float z);

float calcZposB(float y, float z);

int mapf(float angle);

int imapf(float angle);

void moveServo(int topos0, int topos1, int topos2, int  topos3, int topos4, int topos8, int topos5, int topos9, int topos6, int topos10, int topos7, int topos11, int duration);

void moveArm(float Xpos1, float Zpos1, float Ypos1, float Xpos2, float Zpos2, float Ypos2, float Xpos3, float Zpos3, float Ypos3, float Xpos4, float Zpos4, float Ypos4, int vitesse);

void fastArm(float Xpos1,float Zpos1,float Xpos2, float Zpos2, float Xpos3, float Zpos3, float Xpos4, float Zpos4);

void marche();

void cours();

void gauche();

void droite();

void regardG();

void regardD();

void initialisation();

void testpos();

void attente();

void assis();

void vague();

void couche();

unsigned long measureDistance(int triggerPin, int echoPin);
