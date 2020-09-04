//https://icircuit.net/arduino-esp32-home-automation-simple-off-control-using-mqtt/2272


#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define Len 16

// Update these with values suitable for your network.
const char* ssid = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
//const char* mqtt_server = "iot.eclipse.org";
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define mqtt_port 1883
#define MQTT_USER "Sajjad"
#define MQTT_PASSWORD "#sajjan2"
#define MQTT_SERIAL_PUBLISH_CH "esp32/gpio1"
#define MQTT_SERIAL_RECEIVER_CH "esp32/gpio2"
#define ID_MAP_LENGTH 5
WiFiClient wifiClient;
/*id - GPIO
 * 1 - 4
 * 2 - 5
 * 3 - 16
 * 4 - 17
 */
int iomap[ID_MAP_LENGTH]={0,5,4,16,17};
//int iomap[ID_MAP_LENGTH]={0,4,5,16,17};
char buf00[Len]; // buffer array 1
char buf01[Len]; // buffer array 2
boolean recPressed,fill00,fill01,dState;
unsigned int bufByteCount,recByteCount;
byte bufWrite;
PubSubClient client(wifiClient);
 
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
char str_payload[11];//11000];


void ICACHE_RAM_ATTR onTimerISR(){
  //timer1_write(6250); //125us
  timer1_write(5000000); //1s=.02us x 5000000
  //digitalWrite(5,!(digitalRead(5)));  //Toggle LED Pin
  //Serial.println("LED Toggled!!");
  

  char ad = (char)recByteCount+48;//analogRead(A0);
  switch(dState)
  {
    case 0:
      buf00[bufByteCount] = ad;
      if(bufByteCount == Len)
      {
        bufByteCount=0;
        buf01[bufByteCount] = ad;
        fill00 = true;
        dState = 1;
        Serial.println("bif00 filled");
        if (client.publish("esp32/gpio1",buf00) == true) {
          Serial.println("Success sending message buf00");
        } else {
          Serial.println("Error sending message buf00");
        }
      }
      break;
    case 1:
      buf01[bufByteCount] = ad;
      if(bufByteCount == Len)
      {
        bufByteCount=0;
        buf00[bufByteCount] = ad;
        fill01 = true;
        dState = 0;
        Serial.println("bif01 filled");
        if (client.publish("esp32/gpio1",buf01) == true) {
          Serial.println("Success sending message buf01");
        } else {
          Serial.println("Error sending message buf00");
        }
      }
      break;
  }
  bufByteCount++;
  recByteCount++;
/*  if ((bufByteCount == Len) && bufWrite == false) { 
    bufByteCount = 0;
    bufWrite = true;

  } else if ((bufByteCount == Len) & bufWrite == true) {
    bufByteCount = 0;
    bufWrite = false;
  }

  if (bufWrite == false) { buf00[bufByteCount] = ad; }
  if (bufWrite == true) { buf01[bufByteCount] = ad; }
*/
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
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      //client.publish("/icircuit/presence/ESP32/", "hello world");
      client.publish("esp32/gpio1", "hello world");
      //test 2
      char JSONmessageBuffer[100];
      JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println("Sending message to MQTT topic..");
      Serial.println(JSONmessageBuffer);
     
      if (client.publish("esp32/gpio1", JSONmessageBuffer) == true) {
        Serial.println("Success sending message");
      } else {
        Serial.println("Error sending message");
      }
      //Test publish again
        client.publish("esp32/gpio1",str_payload);

      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

unsigned char state;
void callback(char* topic, byte *payload, unsigned int length) {
  //print recevied messages on the serial console
  /*StaticJsonBuffer<500> jsonBuffer;
  JsonObject& message = jsonBuffer.parseObject((char *)payload);
  JsonArray& switches=message["switches"];
  int i=0,id,io_status;
  switch(state)
  {
    case 0:*/
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
        digitalWrite(iomap[id],io_status);
      }
 /*     state = 1;
      break;
    case 1:
      client.publish("esp32/gpio1", "hello world2");
      Serial.println("-------new message from broker2-----");
      Serial.print("channel:");
      Serial.println(topic);
      Serial.print("data:");  
      Serial.write(payload, length);
      Serial.println();
      //StaticJsonBuffer<500> jsonBuffer;
      //JsonObject& message = jsonBuffer.parseObject((char *)payload);
      if (!message.success()) {
        Serial.println("JSON parse failed");  
        return;
      }
      // loop through each switch and swith IO sate
      //JsonArray& switches=message["switches"];
      //int i=0,id,io_status;
      for(i=0;i<switches.size();i++){
        id=switches[i]["id"];
        io_status=switches[i]["status"];
        digitalWrite(iomap[id],io_status);
      }
      state = 2;
      break;
   case 2:
      break;
  }*/
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 

  //json
    JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Temperature";
  JsonArray& values = JSONencoder.createNestedArray("values");
 
  values.add(20);
  values.add(21);
  values.add(23);
  ///////////////
  for(int i=0;i<11;i++)
    {
      str_payload[i]= char('A'+i);
    }
  ///////////////
  int i=0;
  for(i=0;i<ID_MAP_LENGTH;i++){
    pinMode(iomap[i],OUTPUT);
  }
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
/*
    //timer
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
  //timer1_write(6250); //125us=.02us x 6250
  timer1_write(5000000); //1s=.02us x 5000000
  recPressed = true;*/
}

void loop() {
/*  if(fill00==true)
  {
    Serial.println("fill00 Here");
    if (client.publish("esp32/gpio1",buf00) == true) {
      Serial.println("Success sending message buf00");
    } else {
      Serial.println("Error sending message buf00");
    }
    fill00 = false;
  }
  if(fill01==true)
  {
    Serial.println("fill01 Here");
    if (client.publish("esp32/gpio1",buf01) == true) {
      Serial.println("Success sending message buf01");
    } else {
      Serial.println("Error sending message buf01");
    }
    fill01 = false;
  }
*/
   client.loop();
   yield();
 }
 /*
.......
WiFi connected
IP address: 
192.168.0.18
Attempting MQTT connection...connected
Sending message to MQTT topic..
{"device":"ESP32","sensorType":"Temperature","values":[20,21,23]}
Success sending message
-------new message from broker1-----
channel:esp32/gpio2
data:{"switches":[{"status":true,"id":1,"lable":"GPIO 4"},{"status":false,"id":2,"lable":"GPIO 5"},{"status":false,"id":3,"lable":"GPIO 16"},{"status":false,"id":4,"lable":"GPIO 17"}]}
-------new message from broker1-----
channel:esp32/gpio2
data:{"switches":[{"status":false,"id":1,"lable":"GPIO 4"},{"status":false,"id":2,"lable":"GPIO 5"},{"status":false,"id":3,"lable":"GPIO 16"},{"status":false,"id":4,"lable":"GPIO 17"}]}




...........
WiFi connected
IP address: 
192.168.0.18
Attempting MQTT connection...connected
Sending message to MQTT topic..
{"device":"ESP32","sensorType":"Temperature","values":[20,21,23]}
Success sending message
-------new message from broker1-----
channel:esp32/gpio2
data:switches status true id 1 lable GPIO 4 status false id 2 lable GPIO 5 status false id 3 lable GPIO 16 status false id 4 lable GPIO 17 },{"status":false,"id":4,"lable":"GPIO 17"}]}
-------new message from broker2-----
channel:
data:o1hello world2s false id 1 lable GPIO 4 status false id 2 lable GPIO 5 status false id 3 lable GPIO 16 status false id 4 lable GPIO 17 },{"status":false,"id":4,"lable":"GPIO 17"}]}
*/
