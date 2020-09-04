//http://arduino-er.blogspot.com/2014/09/communication-betweeen-arduinos-using.html

//SPISlave
#include <SPI.h>
 


#define CS 15 //define chip select line for manual control
byte dataEcho;  //echo back input data in next round
byte dataToPC;  //send input data to PC

void setup() {
    Serial.begin(115200);  //link to PC
    
    //The Port B Data Direction Register
    DDRB  |= 0b00010000; 
   //The Port B 
    PORTB |= 0b00000100;
    
    //SPI Control Register
    SPCR  |= 0b11000000;
    //SPI status register
    SPSR  |= 0b00000000;
    
    dataEcho = 0;
    dataToPC = 0;
    
    sei();
}

void loop() {
  
  if(dataToPC != 0){
    Serial.write(dataToPC);
    dataToPC = 0;
  }

}

ISR(SPI_STC_vect){
  cli();
  
  //while SS Low
  while(!(PINB & 0b00000100)){
    SPDR = dataEcho;
    
    //wait SPI transfer complete
    while(!(SPSR & (1 << SPIF)));
    
    dataEcho = SPDR;  //send back in next round
  }
  sei();
}
