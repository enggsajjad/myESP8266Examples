//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
//Arduino code
//3. To Send Dynamic Sensor Data from Arduino to NodeMCU


#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(8,9);
//#include "DHT.h"
//#define DHTPIN 2     
//#define DHTTYPE DHT11   
//DHT dht(DHTPIN, DHTTYPE);

void setup() {
s.begin(9600);
delay(5000);
Serial.begin(115200);
Serial.println("Hello Nano");
pinMode(A0,INPUT);
  //dht.begin();
}

StaticJsonBuffer<1000> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
void loop() {
 
  //float h = dht.readHumidity();
  // print a random number from 5 to 29
  float h = random(5, 29);
  Serial.println(h);
  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  float t = random(5, 29);
  Serial.println(t);
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);
  float f = random(5, 29);
  Serial.println(f);
 
  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  float hif = random(5, 29);
  Serial.println(hif);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);
    float hic = random(5, 29);
  Serial.println(hic);
if (isnan(h) || isnan(t) || isnan(f)) {
    return;
  }
  // If the DHT-11 is not connected to correct pin or if it doesnot
//work no data will be sent
  root["temp"] = t;
  root["hum"] = h;
  root["hi"] = hic;
  root["gasv"]= analogRead(A0);
 
if(s.available()>0)
{
 root.printTo(s);
}
}
