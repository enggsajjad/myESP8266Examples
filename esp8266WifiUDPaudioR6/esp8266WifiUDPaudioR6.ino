#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid     = "tecoLecture";
//const char* password = "PerComSS16";
const char* ssid     = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
int contconexion = 0;

WiFiUDP Udp;

#define LED 16  //On board LED
unsigned char cntSec;
unsigned int cnt125us;
float analog1;
unsigned char analog;
boolean recording,recDone;
unsigned char buf0[40960];
unsigned int cnt;
unsigned char state;

#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

void ICACHE_RAM_ATTR VoiceKeyIsPressed();
 
int swVoice = 5;


volatile boolean voiceKeyPressed = false;
volatile boolean voiceFirstEdge = false;
volatile boolean voiceHolding = false;


unsigned long timeVoiceKeyPress = 0;
unsigned long timeVoiceLastPress = 0;



boolean swVoiceState;


unsigned char btnState=0;

void ICACHE_RAM_ATTR onTimerISR()
{
  timer1_write(625); //125us=.2us x 625
  //analog1 = analogRead(A0);
  //analog = map(analog1,0,1023,0,255);
  //buf0[cnt125us++] = analog;
  buf0[cnt125us++] = map(analogRead(A0),0,1023,0,255);
  
  if(cnt125us==(1024*40))
  {
    cnt125us=0;
    timer1_disable();
    recDone = true;
  }
}

void setup()
{
      delay(100);
      Serial.begin(115200);


    Serial.print("Hello IntEnseChol!");
    pinMode(LED, OUTPUT);

    digitalWrite(LED,LOW);

    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  
    timer1_attachInterrupt(onTimerISR);
    //timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    //timer1_write(625); //125us=.2us x 625




    pinMode(swVoice, INPUT);

    attachInterrupt( digitalPinToInterrupt(swVoice), VoiceKeyIsPressed, CHANGE );
    //attachPCINT(digitalPinToPCINT(swAction), ActionKeyIsPressed, CHANGE);//CHANGE);

}

void loop()
{
  yield();
  if(recording)
  {
    Serial.println("Start Speaking");
    delay(2000);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    timer1_write(625); //125us=.2us x 625
    recording = false;
  }


  if(recDone)
  {
    Serial.println("Start Sending");
    for(int j=0; j<40;j++)
    {
      Udp.beginPacket("192.168.0.164", 1234);//my room
      for(int i=0; i<1024;i++)
      {
        Udp.write(buf0[1024*j+i]);
        //yield();
      }
      Udp.endPacket();
     delay(10);
    }
    recDone = false;
    Serial.println("Sending done");
  }

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
          recording = true;
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
