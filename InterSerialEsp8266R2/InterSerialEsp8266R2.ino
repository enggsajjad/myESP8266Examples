//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
String inputString = "";         // a String to hold incoming data
bool stringComplete = false; 
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(4,0);
int data;
void setup() {
s.begin(9600);
delay(5000);
Serial.begin(115200);

Serial.println("Hello ESP8266");
while (!Serial) continue;
}
 
void loop() {
  s.write("s");
 StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;
 
  Serial.println("JSON received and parsed");
  root.prettyPrintTo(Serial);
  Serial.print("Data 1 ");
  Serial.println("");
  int data1=root["data1"];
  Serial.print(data1);
  Serial.print("   Data 2 ");
  int data2=root["data2"];
  Serial.print(data2);
  Serial.println("");
  Serial.println("---------------------xxxxx--------------------");
 
}
