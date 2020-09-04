#include "PinChangeInterrupt.h"

#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

int LED = 13;
int swContext = 2;
int swVoice = 3;
int swAction = 4;
int swTouch = 5;
// variable used for the key press
volatile boolean contextKeyPressed = false;
volatile boolean contextFirstEdge = false;
volatile boolean contextHolding = false;
volatile boolean voiceKeyPressed = false;
volatile boolean voiceFirstEdge = false;
volatile boolean voiceHolding = false;
volatile boolean actionKeyPressed = false;
volatile boolean actionFirstEdge = false;
volatile boolean actionHolding = false;
volatile boolean touchKeyPressed = false;
volatile boolean touchFirstEdge = false;
volatile boolean touchHolding = false;

// variable used for the debounce
unsigned long timeContextKeyPress = 0;
unsigned long timeContextLastPress = 0;
unsigned long timeVoiceKeyPress = 0;
unsigned long timeVoiceLastPress = 0;
unsigned long timeActionKeyPress = 0;
unsigned long timeActionLastPress = 0;
unsigned long timeTouchKeyPress = 0;
unsigned long timeTouchLastPress = 0;


boolean swContextState;
boolean swVoiceState;
boolean swActionState;
boolean swTouchState;

unsigned char state=0;

void setup() 
{
    Serial.begin(115200);
    Serial.print("Hello World!");
    pinMode(LED, OUTPUT);  
    digitalWrite(LED,LOW); 
    pinMode(swContext, INPUT); 
    pinMode(swVoice, INPUT);
 
    attachInterrupt( digitalPinToInterrupt(swContext), ContextKeyIsPressed, CHANGE );
    attachInterrupt( digitalPinToInterrupt(swVoice), VoiceKeyIsPressed, CHANGE );
    attachPCINT(digitalPinToPCINT(swAction), ActionKeyIsPressed, CHANGE);//CHANGE);
    attachPCINT(digitalPinToPCINT(swTouch), TouchKeyIsPressed, CHANGE);//CHANGE);
}
 
void loop()
{
     switch(state)
     {
     case 0:
      break;
     case 1:
     //if (contextKeyPressed)
     //{
          //contextKeyPressed = false;
          state = 0;
          timeContextKeyPress = millis();
          if ( timeContextKeyPress - timeContextLastPress >= DEBOUNCE_TIME)
          {
              switch(swContextState){
              case 0:
                Serial.println("Key Pressed:   ");
                //contextFirstEdge = true;
                state = 2;
                swContextState = 1;
                break;
              case 1:
                if(contextHolding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key ContextHolding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                contextHolding = false;
                //contextFirstEdge = false;
                state = 0;
                swContextState = 0;
                break;
              }//switch                
          }//if time
          timeContextLastPress = timeContextKeyPress;
     //}//if keypressed
      break;
     case 2:
     //if (contextFirstEdge == true)
     //{
       if ( millis() - timeContextLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key ContextHolding start:   ");
          digitalWrite(LED, HIGH);  
          contextHolding = true;
          //contextFirstEdge = false; 
          state = 0;
       }
     //}//ContextFirstEdge
      break;
     case 3:
     //if (voiceKeyPressed)
     //{
          //voiceKeyPressed = false;
          state = 0;
          timeVoiceKeyPress = millis();
          if ( timeVoiceKeyPress - timeVoiceLastPress >= DEBOUNCE_TIME)
          {
              switch(swVoiceState){
              case 0:
                Serial.println("Key Pressed:   ");
                //voiceFirstEdge = true;
                state = 4;
                swVoiceState = 1;
                break;
              case 1:
                if(voiceHolding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key VoiceHolding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                voiceHolding = false;
                //voiceFirstEdge = false;
                state = 0;
                swVoiceState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeVoiceLastPress = timeVoiceKeyPress;
     //}//if keypressed
      break;
     case 4:
     //if (voiceFirstEdge == true)
     //{
       if ( millis() - timeVoiceLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key VoiceHolding start:   ");
          digitalWrite(LED, HIGH);  
          voiceHolding = true;
          //VoiceFirstEdge = false; 
          state = 0;
       }
     //}//VoiceFirstEdge
      break;
     case 5:
     //if (actionKeyPressed)
     //{
          //actionKeyPressed = false;
          state = 0;
          timeActionKeyPress = millis();
          if ( timeActionKeyPress - timeActionLastPress >= DEBOUNCE_TIME)
          {
              switch(swActionState){
              case 0:
                Serial.println("Key Pressed:   ");
                //actionFirstEdge = true;
                state = 6;
                swActionState = 1;
                break;
              case 1:
                if(actionHolding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key ActionHolding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                actionHolding = false;
                //actionFirstEdge = false;
                state = 0;
                swActionState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeActionLastPress = timeActionKeyPress;
     //}//if keypressed
      break;
     case 6:
     //if (actionFirstEdge == true)
     //{
       if ( millis() - timeActionLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key ActionHolding start:   ");
          digitalWrite(LED, HIGH);  
          actionHolding = true;
          //actionFirstEdge = false; 
          state = 0;
       }
     //}//ActionFirstEdge
      break;
     case 7:
     //if (touchKeyPressed)
     //{
          //touchKeyPressed = false;
          state = 0;
          timeTouchKeyPress = millis();
          if ( timeTouchKeyPress - timeTouchLastPress >= DEBOUNCE_TIME)
          {
              switch(swTouchState){
              case 0:
                Serial.println("Key Pressed:   ");
                //touchFirstEdge = true;
                state = 8;
                swTouchState = 1;
                break;
              case 1:
                if(touchHolding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key TouchHolding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                touchHolding = false;
                //touchFirstEdge = false;
                state = 0;
                swTouchState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeTouchLastPress = timeTouchKeyPress;
     //}//if keypressed
      break;
     case 8:
     //if (touchFirstEdge == true)
     //{
       if ( millis() - timeTouchLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key TouchHolding start:   ");
          digitalWrite(LED, HIGH);  
          touchHolding = true;
          //TouchFirstEdge = false; 
          state = 0;
       }
     //}//TouchFirstEdge
      break;
}//switch
}//loop
 
 
void ContextKeyIsPressed()
{
   //contextKeyPressed = true;
   state = 1;
}
void VoiceKeyIsPressed()
{
   //voiceKeyPressed = true;
   state = 3;
}
void ActionKeyIsPressed()
{
   //actionKeyPressed = true;
   state = 5;
}
void TouchKeyIsPressed()
{
   //touchKeyPressed = true;
   state = 7;
}
