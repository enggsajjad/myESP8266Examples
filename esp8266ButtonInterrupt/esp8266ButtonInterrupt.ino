//#include "PinChangeInterrupt.h"


#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

void ICACHE_RAM_ATTR VoiceKeyIsPressed();

const int LED = 12;
 
int swVoice = 5;


volatile boolean voiceKeyPressed = false;
volatile boolean voiceFirstEdge = false;
volatile boolean voiceHolding = false;


unsigned long timeVoiceKeyPress = 0;
unsigned long timeVoiceLastPress = 0;



boolean swVoiceState;


unsigned char btnState=0;


void setup() 
{
  delay(100);
    Serial.begin(115200);
    while (!Serial) {
      delay(1); // will pause Zero, Leonardo, etc until serial console opens
    }
    Serial.print("Hello IntEnseChol!");
    pinMode(LED, OUTPUT);

    digitalWrite(LED,LOW);

    pinMode(swVoice, INPUT);

    attachInterrupt( digitalPinToInterrupt(swVoice), VoiceKeyIsPressed, CHANGE );
    //attachPCINT(digitalPinToPCINT(swAction), ActionKeyIsPressed, CHANGE);//CHANGE);

}


void loop()
{
     switch(btnState)
     {
     case 0://idle
      break;
     ///////////////////////////////////////////////////////////////////////////////
     case 3://voiceKeyPressed
          btnState = 0;
          timeVoiceKeyPress = millis();
          if ( timeVoiceKeyPress - timeVoiceLastPress >= DEBOUNCE_TIME)
          {
              switch(swVoiceState){
              case 0:
                Serial.println("Key Pressed:   ");// KeyPressed is detected.
                btnState = 4;
                swVoiceState = 1;
                break;
              case 1:
                if(voiceHolding)
                {
                  //digitalWrite(LED, LOW);  
                  Serial.println("Key VoiceHolding end:   ");//Key Holding ends here

                }else
                {
                  Serial.println("Key Un-Pressed:   ");  // Key un-Pressed is detected.

                }
                voiceHolding = false;
                btnState = 0;
                swVoiceState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeVoiceLastPress = timeVoiceKeyPress;
      break;
     case 4:
       if ( millis() - timeVoiceLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key VoiceHolding start:   ");//Key Holding starts here

          //digitalWrite(LED, HIGH);  
          voiceHolding = true;
          btnState = 0;
       }
      break;
    ///////////////////////////////////////////////////////////////////////////////
    case 9:
      break;
}//switch
}//loop
void ICACHE_RAM_ATTR VoiceKeyIsPressed()
{
   btnState = 3;
}
