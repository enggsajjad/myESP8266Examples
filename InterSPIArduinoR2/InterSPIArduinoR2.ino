//http://arduino-er.blogspot.com/2014/09/communication-betweeen-arduinos-using.html

//SPIMaster

#include <SPI.h>
byte dataOut;
byte dataIn;

int pinSS = 10;  //Slave Select, active LOW

void setup(){
  Serial.begin(115200);  //link to PC
  
  pinMode(pinSS, OUTPUT);
  digitalWrite(pinSS, HIGH);
  SPI.begin();
}

void loop(){
  while(Serial.available() > 0){
    dataOut = Serial.read();
    dataIn = spiWriteAndRead(dataOut);
    Serial.write(dataIn);
  }
}

byte spiWriteAndRead(byte dout){
  byte din;
  digitalWrite(pinSS, LOW);
  delay(1);
  din = SPI.transfer(dout);
  digitalWrite(pinSS, HIGH);
  return din;
}
