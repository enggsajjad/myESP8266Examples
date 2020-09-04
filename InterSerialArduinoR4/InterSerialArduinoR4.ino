#include <SoftwareSerial.h>
//https://mybtechprojects.tech/serial-communication-between-nodemcu-and-arduino/
//Arduino code

SoftwareSerial s(8,9);
 const int interruptPin = 6;
void setup() {
s.begin(9600);
delay(5000);
Serial.begin(115200);
Serial.println("Hello Nano");

    pinMode(interruptPin, OUTPUT);  
    digitalWrite(interruptPin,LOW); 
    delay(100);
    digitalWrite(interruptPin,HIGH); 
    delay(1000);

    digitalWrite(interruptPin,LOW); 
    delay(100);
    digitalWrite(interruptPin,HIGH); 
    delay(1000);

    digitalWrite(interruptPin,LOW); 
    delay(100);
    digitalWrite(interruptPin,HIGH); 
    delay(1000);
}
 
void loop() {
int data=55;
if(s.available()>0)
{
 s.write(data);
}
}
