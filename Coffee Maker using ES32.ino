/*#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial gsmSerial(0, 1);

int flamePin = A0;
int buzzerPin = 9;
int flameThreshold = 500;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(flamePin, INPUT);
  gsmSerial.begin(9600);  
  Serial.begin(9600);  

  delay(2000); 
  gsmSerial.println("AT");  
  delay(1000);  
  gsmSerial.println("AT+CMGF=1");  
  delay(1000);  
  gsmSerial.println("AT+CSCS=\"GSM\"");  
  delay(1000);  
  gsmSerial.println("AT+CNMI=2,2,0,0,0");  
  delay(1000);  
}

void loop() {
  int flameValue = analogRead(flamePin);

  if (flameValue > flameThreshold) {
    digitalWrite(buzzerPin, HIGH);
    sendSMS();
    delay(5000); 
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

void sendSMS() {
  gsmSerial.println("AT+CMGS=\"+639517165632\"");
  delay(100);
  gsmSerial.println("Fire Alert! Flame detected!"); 
  delay(100);
  gsmSerial.write(26);  
  delay(5000);
}*/

//KURUNOTCHI

#include <Arduino.h> //Di na need i-copy kay naka platformio kasi ako
#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(7, 8);

int flamePin = A0;
int buzzerPin = 9;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(flamePin, INPUT);
  gsmSerial.begin(9600);  
  Serial.begin(9600);  

  delay(2000); 
  gsmSerial.println("AT");  
  delay(1000);  
  gsmSerial.println("AT+CMGF=1");  
  delay(1000);  
  gsmSerial.println("AT+CSCS=\"GSM\"");  
  delay(1000);  
  gsmSerial.println("AT+CNMI=2,2,0,0,0");  
  delay(1000);  
}

void loop() {
  int flameValue = analogRead(flamePin);

  if (flameValue > 500) {
    digitalWrite(buzzerPin, HIGH);
    makeCall();
    delay(5000); 
  } else {
    digitalWrite(buzzerPin, LOW);
    hangUp();
  }
}

void makeCall() {
  gsmSerial.println("ATD+639517165632;"); 
  delay(10000); 
}

void hangUp() {
  gsmSerial.println("ATH");
  delay(1000);
}
