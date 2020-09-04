/*================================= EG LABS =======================================

Loop Back test for the SPI port of the Arduino

 

 The circuit:

 * LED attached from pin 6 to ground through a 1K resistor

================================== EG LABS =======================================*/

 

#include <SPI.h>                                                  // including the SPI library


char outByte [20] = "ENGINEERS GARAGE";                           // string to be send and received via SPI port

int led = 6;                                                      // variable which holds the pin number at which the LED is attached

char inByte;                                                      // variable which stores the value of the byte received from SPI bus

int i = 0;                                                         


void setup() 

{

  pinMode(led, OUTPUT);                                           // setting the LED pin as output

  Serial.begin(115200);                                             // initializing the serial port at 9600 baud rate

  SPI.begin();                                                    // initialize the SPI port as master

  delay(100);  

}


void loop() 

{    

  digitalWrite(led, HIGH);          

  for(i = 0; outByte [i] != '\0'; i ++)                            // send and receive the bytes of the string

  {

    inByte = SPI.transfer(outByte [i]);

    Serial.write(inByte);   

  }

  Serial.println();

  

  delay(1000);

  digitalWrite(led, LOW); 

  delay(1000);  

}
