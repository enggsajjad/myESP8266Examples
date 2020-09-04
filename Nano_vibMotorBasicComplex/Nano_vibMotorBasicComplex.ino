#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

// Define the pins being used
int pin_LED = 13;
int pin_switch = 2;
 
// variable used for the key press
volatile boolean keyPressed = false;
 
// variable used to control the LED
boolean LEDstatus = LOW;
 
// variable used for the debounce
unsigned long timeNewKeyPress = 0;
unsigned long timeLastKeyPress = 0;
unsigned int timeDebounce = 10;
unsigned char effect = 84;
void setup() {
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
  drv.setWaveform(0, effect);  // ramp up medium 1, see datasheet part 11.2
  drv.setWaveform(1, 1);  // strong click 100%, see datasheet part 11.2
  drv.setWaveform(2, 0);  // end of waveforms
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
              drv.setWaveform(0, effect);  // ramp up medium 1, see datasheet part 11.2
              //drv.setWaveform(1, 1);  // strong click 100%, see datasheet part 11.2
              drv.setWaveform(2, 0);  // end of waveforms 
              
          }
          timeLastKeyPress = timeNewKeyPress;
     }
     
    drv.go();
    delay(1000);

}

void keyIsPressed()
{
   keyPressed = true;
}
 
void blink()
{
      if (LEDstatus == LOW) { LEDstatus = HIGH; } else { LEDstatus = LOW; }   
      digitalWrite(pin_LED, LEDstatus);
}
