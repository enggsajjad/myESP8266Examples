//  Sketch: SwitchingThings_Interuupt_02
//
//  An  example of using a, interrupt and a button switch to turn an LED on and off
//  Now with rudimentary debounce
//
//  Pins
//  D10 to resister and LED
//  D2 to push button switch
//  
 
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
unsigned long timePressed = 0;
unsigned long timeUnpressed = 0;
unsigned int timeDebounce = 10;
unsigned char btnState;
void setup() 
{
    Serial.begin(115200);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    pinMode(pin_LED, OUTPUT);  
    digitalWrite(pin_LED,LOW); 
    pinMode(pin_switch, INPUT); 
 
    attachInterrupt( digitalPinToInterrupt(pin_switch), keyIsPressed, CHANGE );
}
 
void loop()
{
     if (keyPressed)
     {
          keyPressed = false;
          ////////////
          timeNewKeyPress = millis();
          
          
          
          if ( timeNewKeyPress - timeLastKeyPress >= timeDebounce)
          {
              //blink();
              switch(btnState){
              case 0:
                digitalWrite(pin_LED,HIGH); 
                Serial.println("Key Pressed:   ");
                digitalWrite(pin_LED, HIGH);  

                btnState = 1;
                break;
              case 1:
                digitalWrite(pin_LED,LOW); 
                Serial.println("Key Un-Pressed:   ");  
                digitalWrite(pin_LED, LOW);
                btnState = 0;
                break;
              case 2:
                break;
              }                

          }
          timeLastKeyPress = timeNewKeyPress;
     }
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
