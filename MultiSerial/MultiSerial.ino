/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.
*/


void setup() {
  delay(1000);
  // initialize both serial ports:
  Serial.begin(115200,SERIAL_8N1,3,1);//rx,tx
  
  Serial1.begin(115200,SERIAL_8N1,0,2);
  Serial2.begin(115200,SERIAL_8N1,19,18);
  Serial.print("Hussain");
  Serial1.print("Sajjad");
  Serial2.print("Islamabad");
}

void loop() {
  // read from port 1, send to port 0:
  if (Serial2.available()> 0) {
    int inByte = Serial2.read();
    Serial.print('3');
    Serial.print(char(inByte));
  }

  // read from port 1, send to port 0:
  if (Serial1.available()> 0) {
    int inByte = Serial1.read();
    Serial.print('1');
    Serial.print(char(inByte));
  }

  // read from port 0, send to port 1:
  if (Serial.available()> 0) {
    int inByte = Serial.read();
    Serial.print('2');
    Serial1.print(char(inByte));
  }
}
