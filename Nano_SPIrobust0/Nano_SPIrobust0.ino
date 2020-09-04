/*************************************************************
 SPI_Raspi_Arduino
   Configures an ATMEGA as an SPI slave and demonstrates
   a basic bidirectional communication scheme
   A Raspberry Pi SPI master transmits commands to 
   perform addition and subtraction on a pair of integers and
   the Ardunio transmits the result
****************************************************************/

/***************************************************************
 Global Variables
  -receiveBuffer[] and dat are used to capture incoming data
   from the Raspberry Pi
  -marker is used as a pointer to keep track of the current
   position in the incoming data packet
***************************************************************/
#include <SPI.h>
unsigned char receiveBuffer[5];
unsigned char dat;
byte marker = 0;


/*************************************************************
 Unions allow variables to occupy the same memory space a 
 convenient way to move back and forth between 8-bit and 
 16-bit values etc.  Here three unions are declared: 
 two for parameters that are passed in commands to the Arduino 
 and one to receive  the results 
 ***************************************************************/


  int p1Int;
  int p2Int;
  int resultInt;


/***************************************************************  
 Setup SPI in slave mode (1) define MISO pin as output (2) set
 enable bit of the SPI configuration register 
****************************************************************/ 

void setup (void)
{
  Serial.begin(115200);
  Serial.println("Hello Sketch:   ");
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);// turn on SPI in slave mode
  SPI.attachInterrupt(); // turn on interrupt
}  


/***************************************************************  
 Loop until the SPI End of Transmission Flag (SPIF) is set
 indicating a byte has been received.  When a byte is
 received, call the spiHandler function.
****************************************************************/

void loop (void)
{

/*
  if((SPSR & (1 << SPIF)) != 0)
  {
    spiHandler();
   }
*/
}  
ISR (SPI_STC_vect) // SPI interrupt routine 
{ 
  spiHandler();
}
/***************************************************************  
 spiHandler
   Uses the marker variable to keep track current position in the
   incoming data packet and execute accordingly
   0   - wait for to receive start byte - once received send
         the acknowledge byte
   1   - the command to add or subtract
   2-5 - two integer parameters to be added or subtracted
       - when the last byte (5) is received, call the
         executeCommand function and load the first byte of the
         result into SPDR
   6   - transmit the first byte of the result and load the 
         second byte into SPDR
   7   - transmit the second byte of of the result and reset
         the marker   
****************************************************************/


void spiHandler()
{
  switch (marker)
  {
  case 0:
    dat = SPDR;
    if (dat == 'c')
    {
      SPDR = 'a';
      marker++;
    } 
    break;    
  case 1:
    receiveBuffer[marker-1] = SPDR;
    //Serial.print("Commands ");Serial.println(receiveBuffer[0],DEC);
    marker++;
    break;
  case 2:
    receiveBuffer[marker-1] = SPDR;
    //Serial.print("Int1.MSB ");Serial.println(receiveBuffer[1],DEC);
    marker++;
    break;
  case 3:
    receiveBuffer[marker-1] = SPDR;
    //Serial.print("Int1.LSB ");Serial.println(receiveBuffer[2],DEC);
    marker++;
    break;
  case 4:
    receiveBuffer[marker-1] = SPDR;
    //Serial.print("Int2.MSB ");Serial.println(receiveBuffer[3],DEC);
    marker++;
    break;
  case 5:
    receiveBuffer[marker-1] = SPDR;
    //Serial.print("Int2.LSB ");Serial.println(receiveBuffer[4],DEC);
    marker++;
    executeCommand();
    SPDR = (resultInt>>8)&0xff;
    break;    
  case 6:
    marker++;
    SPDR = (resultInt)&0xff;
    break;   
  case 7:
    dat = SPDR;
    marker=0;
  }

}

/***************************************************************  
 executeCommand
   When the complete 5 byte command sequence has been received
   reconstitute the byte variables from the receiveBuffer
   into integers, parse the command (add or subtract) and perform
   the indicated operation - the result will be in resultBuffer
****************************************************************/

  
void executeCommand(void)
{

 p1Int = receiveBuffer[1];
 p1Int = (p1Int<<8) + receiveBuffer[2];
 //Serial.print("Int1 ");Serial.println(p1Int,DEC);
 p2Int = receiveBuffer[3];
 p2Int = (p2Int<<8) + receiveBuffer[4];
 //Serial.print("Int2 ");Serial.println(p2Int,DEC);
  
 if(receiveBuffer[0] == 'a')
 {
   resultInt = p1Int + p2Int;
   //Serial.print("resultInt ");Serial.println(resultInt,DEC);
   //Serial.print("resultIntMSB ");Serial.println((resultInt>>8)&0xff,DEC);
   //Serial.print("resultIntLSB ");Serial.println((resultInt)&0xff,DEC);
  }
 else if (receiveBuffer[0] == 's')
 {
  resultInt = p1Int - p2Int;
  //Serial.print("resultInt ");Serial.println(resultInt,DEC);
  //Serial.print("resultIntMSB ");Serial.println((resultInt>>8)&0xff,DEC);
  //Serial.print("resultIntLSB ");Serial.println((resultInt)&0xff,DEC);
 }

}
