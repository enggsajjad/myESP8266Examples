//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
//3. To Send Dynamic Sensor Data from Arduino to NodeMCU
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
  {
    return;
  }
  //Print the data in the serial monitor
  Serial.println("JSON received and parsed");
  root.prettyPrintTo(Serial);
  Serial.println("");
  Serial.print("Temperature ");
  int data1=root["temp"];
  Serial.println(data1);
  Serial.print("Humidity    ");
  int data2=root["hum"];
  Serial.println(data2);
  Serial.print("Heat-index  ");
  int data3=root["hi"];
  Serial.println(data3);
  Serial.print("gas sensor  ");
  int data4=root["gasv"];
  Serial.println(data4);
  Serial.println("");
  Serial.println("---------------------xxxxx--------------------");
 Serial.println("");
}
