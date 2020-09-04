//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
//Arduino code
//2. Sending Multiple data between NodeMCU and Arduino:

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(8,9);
 
void setup() {
s.begin(9600);
delay(5000);
Serial.begin(115200);
Serial.println("Hello Nano");
}
 
void loop() {
 StaticJsonBuffer<1000> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
  root["data1"] = 100;
  root["data2"] = 200;
if(s.available()>0)
{
 root.printTo(s);
}
}
