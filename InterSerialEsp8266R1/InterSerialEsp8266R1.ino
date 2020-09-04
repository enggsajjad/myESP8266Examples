//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
String inputString = "";         // a String to hold incoming data
bool stringComplete = false; 
#include <SoftwareSerial.h>
SoftwareSerial s(4,0);
void ICACHE_RAM_ATTR isr();
int data;
void setup() {
s.begin(9600);
delay(5000);
Serial.begin(115200);
Serial.println("Hello ESP8266");

pinMode(15, INPUT);//_PULLUP);
attachInterrupt(15, isr, FALLING);
//attachInterrupt( digitalPinToInterrupt(15), isr, FALLING );
    
}
 
void loop() {
  /*
  s.write("s");
  if (s.available()>0)
  {
    data=s.read();
    Serial.println(data);
  }*/
}

void ICACHE_RAM_ATTR isr() {
Serial.println("Hello Pressed");
}
