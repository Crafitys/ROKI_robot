
#include "ROKI.h"
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
extern Adafruit_PWMServoDriver myServo;


#define TRIGGER_PIN_LEFT 14  
#define ECHO_PIN_LEFT 12     
#define TRIGGER_PIN_RIGHT 25 
#define ECHO_PIN_RIGHT 33 

extern int currentAngle[];
unsigned long distanceLeft = 0;
unsigned long distanceRight = 0;
bool isRunning = false;

const char* ssid = "ROKI001"; // Nom du réseau
const char* password = "ROKI001123456"; // Mot de passe du réseau
WebServer server(80);

#define SERVOMIN 500
#define SERVOMAX 2500

void setup() {
  Serial.begin(9600);

  WiFi.softAP(ssid, password);
  Serial.println("Point d'accès démarré");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getPage());
  });
  server.on("/angles", HTTP_GET, []() {
  String json = "[";
  for (int i = 0; i < 12; i++) {
    if (i > 0) json += ",";
    json += String(currentAngle[i]);
  }
  json += "]";
  server.send(200, "application/json", json);
});
server.on("/play", HTTP_GET, []() {
  isRunning = true;
  server.send(200, "text/plain", "Playing");
});

server.on("/pause", HTTP_GET, []() {
  isRunning = false;
  server.send(200, "text/plain", "Paused");
});
server.on("/pause", HTTP_GET, []() {
  isRunning = false;
  server.send(200, "text/plain", "Paused");
});
server.on("/ASSIS", HTTP_GET, []() {
  assis();
  server.send(200, "text/plain", "ASSIS");
});
server.on("/VAGUE", HTTP_GET, []() {
  vague();
  server.send(200, "text/plain", "VAGUE");
});
server.on("/RECHERCHE", HTTP_GET, []() {
  regardG();
  regardD();
  server.send(200, "text/plain", "RECHERCHE");
});
server.on("/COUCHE", HTTP_GET, []() {
  couche();
  server.send(200, "text/plain", "COUCHE");
});
  server.begin();
  server.on("/distances", HTTP_GET, []() {
  String json = "{\"left\": " + String(distanceLeft) + ", \"right\": " + String(distanceRight) + "}";
  server.send(200, "application/json", json);
});
  
  myServo.begin();
  myServo.setPWMFreq(60);
  delay(500);
  pinMode(TRIGGER_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIGGER_PIN_RIGHT, OUTPUT);
  pinMode(ECHO_PIN_RIGHT, INPUT);
  
  initialisation();
  delay(1000);
}

void loop() 
{
  server.handleClient();

  if (isRunning){
    distanceLeft = measureDistance(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT);
    distanceRight = measureDistance(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT);

    if (distanceLeft <30 && distanceRight <30){
      regardG();
      distanceLeft = measureDistance(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT);
      regardD();
      distanceRight = measureDistance(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT);
      if (distanceLeft < distanceRight){
        for (int i =0; i< 5; i++){
           droite();
          delay(50);
        }
      }
     else{
       for (int i=0; i<5; i++){
         gauche();
         delay(50);
       }
     }
    
   }
    else if (distanceLeft && distanceRight > 60){
      marche();
    }

   else if (distanceLeft < 30){
        droite();
        delay(50);
    
    }
   else if (distanceRight < 30){
       gauche();
       delay(50);
     
    }
    else{
      marche();
    }
  }
  else {
    distanceLeft = measureDistance(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT);
    distanceRight = measureDistance(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT);
    attente();
  }
}

String getPage() {
  String page = "<!DOCTYPE html><html><head><title>ESP32 Web Server</title><style>"
                "body {text-align: center; font-family: Arial, sans-serif;}"
                "table {margin: 0 auto; border: 1px solid black; border-collapse: collapse;} "
                "th, td {padding: 10px; border: 1px solid black;} "
                ".jauge {width: 50px; height: 200px; border: 1px solid #000; position: relative; display: inline-block; margin-right: 20px;} "
                ".niveau {width: 100%; height: 0%; background-color: red; position: absolute; bottom: 0;}"
                "button {margin: 10px; padding: 15px 30px; font-size: 16px;}"
                "</style></head><body>"
                "<h1>ROKI ROBOT DOG</h1>"
                "<table id='anglesTable'><tr><th>Servo #</th><th>Angle</th></tr></table>"
                "<button id='playButton'>Play</button><button id='pauseButton'>Pause</button>"
                "<div style='margin-top:20px;'>"
                "<h2>LEFT RIGHT DISTANCES</h2>"
                "<div id='jaugeGauche' class='jauge'>"
                "<div id='niveauGauche' class='niveau'></div>"
                "</div>"
                "<div id='jaugeDroite' class='jauge'>"
                "<div id='niveauDroite' class='niveau'></div>"
                "</div>"
                "</div>"
                "<button id='ASSIS'>ASSIS</button><button id='VAGUE'>VAGUE</button>"
                "<button id='RECHERCHE'>RECHERCHE</button><button id='COUCHE'>COUCHE</button>"
                "</div>"
                "</div>"
                "<script>"
                "document.getElementById('playButton').onclick = function() { fetch('/play').then(() => { console.log('Playing'); }); };"
                "document.getElementById('pauseButton').onclick = function() { fetch('/pause').then(() => { console.log('Paused'); }); };"
                "document.getElementById('ASSIS').onclick = function() { fetch('/ASSIS').then(() => { console.log('ASSIS'); }); };"
                "document.getElementById('VAGUE').onclick = function() { fetch('/VAGUE').then(() => { console.log('VAGUE'); }); };"
                "document.getElementById('RECHERCHE').onclick = function() { fetch('/RECHERCHE').then(() => { console.log('RECHERCHE'); }); };"
                "document.getElementById('COUCHE').onclick = function() { fetch('/COUCHE').then(() => { console.log('COUCHE'); }); };"
                "setInterval(function() {"
                "fetch('/angles').then(response => response.json()).then(data => {"
                "const table = document.getElementById('anglesTable');"
                "let innerHTML = '<tr><th>Servo #</th><th>Angle</th></tr>';"
                "for (let i = 0; i < data.length; i++) {"
                "innerHTML += '<tr><td>' + (i + 1) + '</td><td>' + data[i] + '</td></tr>';"
                "}"
                "table.innerHTML = innerHTML;"
                "});"
                "fetch('/distances').then(response => response.json()).then(data => {"
                "document.getElementById('niveauGauche').style.height = Math.min(data.left / 100 * 200, 200) + 'px';"
                "document.getElementById('niveauDroite').style.height = Math.min(data.right / 100 * 200, 200) + 'px';"
                "});"
                "}, 100);"
                "</script>"
                "</body></html>";
  return page;
}
