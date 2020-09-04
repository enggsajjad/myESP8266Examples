#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

int LED = 13;
int swContext = 2;
int swVoice = 3;
int swAction = 4;
// variable used for the key press
volatile boolean ContextKeyPressed = false;
volatile boolean ContextFirstEdge = false;
volatile boolean ContextHolding = false;

 
// variable used for the debounce
unsigned long timeContextKeyPress = 0;
unsigned long timeContextLastPress = 0;

unsigned char btnState;


void setup() 
{
    Serial.begin(115200);
    Serial.print("Hello World!");
    pinMode(LED, OUTPUT);  
    digitalWrite(LED,LOW); 
    pinMode(swContext, INPUT); 
 
    attachInterrupt( digitalPinToInterrupt(swContext), ContextKeyIsPressed, CHANGE );
}
 
void loop()
{
     if (ContextKeyPressed)
     {
          ContextKeyPressed = false;
          timeContextKeyPress = millis();
          if ( timeContextKeyPress - timeContextLastPress >= DEBOUNCE_TIME)
          {
              switch(btnState){
              case 0:
                Serial.println("Key Pressed:   ");
                ContextFirstEdge = true;
                btnState = 1;
                break;
              case 1:
                if(ContextHolding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key ContextHolding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                ContextHolding = false;
                ContextFirstEdge = false;
                btnState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeContextLastPress = timeContextKeyPress;
     }//if keypressed
     if (ContextFirstEdge == true)
     {
       if ( millis() - timeContextLastPress >= HOLDSTART_TIME)
       {
          Serial.println("Key ContextHolding start:   ");
          digitalWrite(LED, HIGH);  
          ContextHolding = true;
          ContextFirstEdge = false; 
       }
     }
}//loop
 
 
void ContextKeyIsPressed()
{
   ContextKeyPressed = true;
}
