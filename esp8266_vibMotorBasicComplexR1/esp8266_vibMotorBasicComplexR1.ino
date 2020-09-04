#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
void ICACHE_RAM_ATTR keyIsPressed();
// Define the pins being used
int pin_LED = 16;
int pin_switch = 12;
 
// variable used for the key press
volatile boolean keyPressed = false;
 
// variable used to control the LED
boolean LEDstatus = LOW;
 
// variable used for the debounce
unsigned long timeNewKeyPress = 0;
unsigned long timeLastKeyPress = 0;
unsigned int timeDebounce = 10;
unsigned char effect = 84;
unsigned char cntr;
void setup() {
  delay(100);
  Serial.begin(115200);
  Serial.println("DRV test");

    Serial.println("Hello Sajjad!");
    
 
    pinMode(pin_LED, OUTPUT);  
    digitalWrite(pin_LED,LOW); 
    pinMode(pin_switch, INPUT); 
 
    attachInterrupt( digitalPinToInterrupt(pin_switch), keyIsPressed, RISING );
    
  drv.begin();
    
  // I2C trigger by sending 'go' command 
  drv.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command

  drv.selectLibrary(1);

}

void loop() {
       if (keyPressed)
     {
          keyPressed = false;
          timeNewKeyPress = millis();
 
          if ( timeNewKeyPress - timeLastKeyPress >= timeDebounce)
          {
              blink();
              effect = effect - 10;
              Serial.print("Effect");
              Serial.println(effect,DEC);
              /*
              drv.setWaveform(0, 24);
              drv.setWaveform(1, 34);
              drv.setWaveform(2, 14);
              drv.setWaveform(3, 4);
              drv.setWaveform(4, 44);
              drv.setWaveform(5, 24);
              drv.setWaveform(6, 34);
              drv.setWaveform(7, 0);
              drv.go();
              delay(5000);
              */
              for(cntr=0;cntr<50;cntr++)
              {
                drv.setWaveform(0, 37);
                drv.setWaveform(1, 0);
                drv.go();
                delay(50);
              }
              
              delay(5000);

              for(cntr=0;cntr<50;cntr++)
              {
                drv.setWaveform(0, 37);
                drv.setWaveform(1, 0);
                drv.go();
                delay(10);
              }
              
          }
          timeLastKeyPress = timeNewKeyPress;
     }
     
    drv.go();
    delay(2000);

}

void ICACHE_RAM_ATTR keyIsPressed()
{
   keyPressed = true;
}
 
void blink()
{
      if (LEDstatus == LOW) { LEDstatus = HIGH; } else { LEDstatus = LOW; }   
      digitalWrite(pin_LED, LEDstatus);
}
