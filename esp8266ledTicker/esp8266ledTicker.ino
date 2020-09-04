#include <ESP8266WiFi.h>
#include <Ticker.h>  //Ticker Library

Ticker blinker;
Ticker blinker1;

#define ESP8266_LED 5
#define GREEN_PIN 12
#define BLUE_PIN 13
#define RED_PIN 16
void setup() 
{
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  delay(5000);
  Serial.begin(115200);
  Serial.println("Hello");
  blinker.attach(1, changeState); //Use <strong>attach_ms</strong> if you need time in ms
  blinker1.attach(1, changeState1); //Use <strong>attach_ms</strong> if you need time in ms

}

void changeState()
{
  digitalWrite(ESP8266_LED, !(digitalRead(ESP8266_LED)));  //Invert Current State of LED  
}

void changeState1()
{
  digitalWrite(RED_PIN, !(digitalRead(RED_PIN)));  //Invert Current State of LED  
}

void loop() 
{
}
