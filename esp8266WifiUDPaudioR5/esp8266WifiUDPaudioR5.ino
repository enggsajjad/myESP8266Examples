#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid     = "tecoLecture";
//const char* password = "PerComSS16";
const char* ssid     = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
int contconexion = 0;

WiFiUDP Udp;

#define LED 5  //On board LED
unsigned char cntSec;
unsigned int cnt125us;
float analog1;
unsigned char analog;
boolean recording=true,recDone;
unsigned char buf0[40960];
unsigned int cnt;
unsigned char state;

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
  Serial.begin(115200);
  Serial.println();

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  /*while (WiFi.status() != WL_CONNECTED and contconexion <50) {
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      digitalWrite(5, HIGH);  
  }
  else { 
      Serial.println("");
      Serial.println("Connection failed");
      digitalWrite(5, LOW);
  }*/
    while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  
    timer1_attachInterrupt(onTimerISR);
    //timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    //timer1_write(625); //125us=.2us x 625
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
    recording = true;
  }

}
