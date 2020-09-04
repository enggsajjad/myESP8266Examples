#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

int LED = 13;
int swContext = 2;
 
// variable used for the key press
volatile boolean keyPressed = false;
volatile boolean firstEdge = false;
volatile boolean holding = false;

 
// variable used for the debounce
unsigned long timeNewKeyPress = 0;
unsigned long timeLastKeyPress = 0;

unsigned char btnState;


void setup() 
{
    Serial.begin(115200);
    Serial.print("Hello World!");
    pinMode(LED, OUTPUT);  
    digitalWrite(LED,LOW); 
    pinMode(swContext, INPUT); 
 
    attachInterrupt( digitalPinToInterrupt(swContext), keyIsPressed, CHANGE );
}
 
void loop()
{
     if (keyPressed)
     {
          keyPressed = false;
          timeNewKeyPress = millis();
          if ( timeNewKeyPress - timeLastKeyPress >= DEBOUNCE_TIME)
          {
              switch(btnState){
              case 0:
                Serial.println("Key Pressed:   ");
                firstEdge = true;
                btnState = 1;
                break;
              case 1:
                if(holding)
                {
                  digitalWrite(LED, LOW);  
                  Serial.println("Key holding end:   ");
                }else
                {
                  Serial.println("Key Un-Pressed:   ");  
                }
                holding = false;
                firstEdge = false;
                btnState = 0;
                break;
              case 2:
                break;
              }//switch                
          }//if time
          timeLastKeyPress = timeNewKeyPress;
     }//if keypressed
     if (firstEdge == true)
     {
       if ( millis() - timeLastKeyPress >= HOLDSTART_TIME)
       {
          Serial.println("Key holding start:   ");
          digitalWrite(LED, HIGH);  
          holding = true;
          firstEdge = false; 
       }
     }
}//loop
 
 
void keyIsPressed()
{
   keyPressed = true;
}
