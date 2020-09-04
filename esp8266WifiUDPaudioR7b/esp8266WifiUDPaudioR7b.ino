#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


const char* ssid     = "tecoLecture";
const char* password = "PerComSS16";
//const char* ssid = "UPC0870375";
//const char* password = "Ghnfdd6byuxp";
const char* mqtt_server = "iot.eclipse.org";//"broker.mqtt-dashboard.com";
#define mqtt_port 1883
#define MQTT_USER "Sajjad"
#define MQTT_PASSWORD "#sajjan2"
#define MQTT_SERIAL_PUBLISH_CH "intensechoi/controller/get"
#define MQTT_SERIAL_RECEIVER_CH "intensechoi/controller/set"

WiFiUDP Udp;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

#define LED 16  //On board LED
unsigned char cntSec;
unsigned int cnt125us;
float analog1;
unsigned char analog;
boolean recording,recDone;
unsigned char buf0[40960];
unsigned int cnt;
unsigned char state;
#define strLen 256
char buf1[strLen];//11000];

#define DEBOUNCE_TIME 10
#define HOLDSTART_TIME 1000

void ICACHE_RAM_ATTR VoiceKeyIsPressed();
 
int swVoice = 13;


volatile boolean voiceKeyPressed = false;
volatile boolean voiceFirstEdge = false;
volatile boolean voiceHolding = false;


unsigned long timeVoiceKeyPress = 0;
unsigned long timeVoiceLastPress = 0;



boolean swVoiceState;


unsigned char btnState=0;

void setup_wifi(void);
void reconnect(void);
void callback(char* topic, byte *payload, unsigned int length);

void ICACHE_RAM_ATTR onTimerISR()
{
  timer1_write(625); //125us=.2us x 625
  //analog1 = analogRead(A0);
  //analog = map(analog1,0,1023,0,255);
  //buf0[cnt125us++] = analog;
  buf0[cnt125us++] = map(analogRead(A0),0,1023,0,255);
  
  if(cnt125us==(1024*40))
  {
    //cnt125us=0;
    timer1_disable();
    //recDone = true;
  }
}

void setup()
{
      delay(100);
      Serial.begin(115200);


    Serial.print("Hello IntEnseChol!");
    pinMode(LED, OUTPUT);

    digitalWrite(LED,LOW);

  //UDP
  /*WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  */
  //MQTT
  for(int i=0;i<strLen;i++)
  {
    buf1[i]= char('A'+(i%26));//65+(i%26)
  }
  buf1[strLen-1]= '\0';
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  
    timer1_attachInterrupt(onTimerISR);
    //timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    //timer1_write(625); //125us=.2us x 625




    pinMode(swVoice, INPUT);

    attachInterrupt( digitalPinToInterrupt(swVoice), VoiceKeyIsPressed, CHANGE );
    //attachPCINT(digitalPinToPCINT(swAction), ActionKeyIsPressed, CHANGE);//CHANGE);
    

}

void loop()
{
  //if (!client.connected())
  //  reconnect();
  client.loop();
  
  yield();
  if(recording)
  {
    recording = false;
    //Serial.println("Start Speaking");
    //delay(2000);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    timer1_write(625); //125us=.2us x 625
    
  }
  if(recDone)
  {
    recDone = false;
    Serial.print("cnt125us ");Serial.println(cnt125us);
    Serial.println("Start Sending");
    for(int jj=0;jj<160;jj++)
    {
      buf1[0]=char((jj/100)+48);
      buf1[1]=char(((jj-(jj/100)*100)/10)+48);
      buf1[2]=char((jj%10)+48);
      client.publish("intensechoi/controller/audio",buf1);
      
    }
    /*for(int j=0; j<40;j++)
    {
      Udp.beginPacket("192.168.0.164", 1234);//my room
      for(int i=0; i<1024;i++)
      {
        Udp.write(buf0[1024*j+i]);
        //yield();
      }
      Udp.endPacket();
      
     delay(10);
    }*/
    
    cnt125us=0;
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
                  
                  //cnt125us=1024*40;
                  timer1_disable();
                  recDone = true;
                  
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


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      //client.publish("/icircuit/presence/ESP32/", "hello world");
      if (client.publish("intensechoi/controller/audio", "hello world")== true) 
      {
        Serial.println("Success sending message");
      } else 
      {
        Serial.println("Error sending message");
      }
        
      //Test publish again
      /*for(int jj=0;jj<160;jj++)
      {
        buf1[0]=char((jj/100)+48);
        buf1[1]=char(((jj-(jj/100)*100)/10)+48);
        buf1[2]=char((jj%10)+48);
        client.publish("intensechoi/controller/audio",buf1);
        
      }*/
        /*delay(1000);
      //Test publish again
        client.publish("intensechoi/controller/audio",buf1);
        */
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  //print recevied messages on the serial console
  Serial.println("-------new message from broker1-----");
  Serial.print("channel:");
  Serial.println(topic);
  Serial.print("data:");  
  Serial.write(payload, length);
  Serial.println();
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& message = jsonBuffer.parseObject((char *)payload);
  if (!message.success()) {
    Serial.println("JSON parse failed");  
    return;
  }
  // loop through each switch and swith IO sate
  JsonArray& switches=message["switches"];
  int i=0,id,io_status;
  for(i=0;i<switches.size();i++){
    id=switches[i]["id"];
    io_status=switches[i]["status"];
    //digitalWrite(iomap[id],io_status);
  }
}
