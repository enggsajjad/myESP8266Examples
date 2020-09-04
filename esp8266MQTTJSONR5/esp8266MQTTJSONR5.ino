//https://icircuit.net/arduino-esp32-home-automation-simple-off-control-using-mqtt/2272


#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define Len 16

// Update these with values suitable for your network.
const char* ssid = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
//const char* ssid     = "tecoLecture";
//const char* password = "PerComSS16";

//const char* mqtt_server = "iot.eclipse.org";
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define mqtt_port 1883
#define MQTT_USER "Sajjad"
#define MQTT_PASSWORD "#sajjan2"
#define PUBLISH__AUDIO_CH "intensechoi/controller/audio"
#define PUBLISH_CH "intensechoi/controller/get"
#define SUBSCRIBE_CH "intensechoi/controller/set"
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

String arr[9];
char * dev;
char * dev1;
String * fString[9];
PubSubClient client(wifiClient);
 
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
char str_payload[11];//11000];


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
      //client.publish(PUBLISH_CH, "hello world");
      //test 2
      char JSONmessageBuffer[100];
      JSONencoder["device"] = "none";
      JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println("Sending message to MQTT topic..");
      Serial.println(JSONmessageBuffer);
     
      if (client.publish(PUBLISH_CH, JSONmessageBuffer) == true) {
        Serial.println("Success sending message");
      } else {
        Serial.println("Error sending message");
      }
      //Test publish again
      //  client.publish(PUBLISH_CH,str_payload);

      // ... and resubscribe
      client.subscribe(SUBSCRIBE_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void parsing( byte *payload)
{
  
  int i;
  StaticJsonBuffer<500> jsonBuffer;
          JsonObject& message = jsonBuffer.parseObject((char *)payload);
          if (!message.success()) {
            Serial.println("JSON parse failed");  
            return;
          }
          //const char * device = message["device"]; //Get sensor type value
          String device1 = message["device"]; //Get sensor type value
          arr[8] = device1;
          //dev = (char *)device;
          Serial.print("device type: ");
          //Serial.println(device);
         //Serial.println(dev);
         //Serial.println(device1);
         fString[8] = &arr[8];//&device1;
         Serial.println(*fString[8]);
          for ( i = 0; i < 8; i++) 
          { //Iterate through results
            String mappingValue = message["mapping"][i];  //Implicit cast
            arr[i] = mappingValue;
            //dev1 = (char *)mappingValue.c_str();
            fString[i] = &arr[i];
            //fString[i] = &mappingValue;//&arr[i];
            //if(arr[i] == "SAJJAD")
            if(*fString[i] == "HUSSAIN")
              Serial.println("sajjad found");
            //Serial.println(mappingValue);
            Serial.println(*fString[i]);
          }

          

}
void subscribing(String devState, String actState = "NULL")
{
            char JSONmessageBuffer[100];
          JSONencoder["device"] = devState;
          if (actState!= "NULL")
            JSONencoder["cmd"] = actState;
          JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
          Serial.println("Sending message to MQTT topic..");
          Serial.println(JSONmessageBuffer);
         
          if (client.publish(PUBLISH_CH, JSONmessageBuffer) == true) {
            Serial.println("Success sending message");
          } else {
            Serial.println("Error sending message");
          }
          
}
unsigned char state;
void callback(char* topic, byte *payload, unsigned int length) {
  char tt;
  //print recevied messages on the serial console
      Serial.println("-------new message from broker1-----");
      Serial.print("channel:");
      Serial.println(topic);
      Serial.print("data:");  
      Serial.write(payload, length);
      Serial.println();
      switch(state)
      {
        case 0:
          parsing(payload);
          for (tt = 0; tt < 9; tt++) //Iterate through results
          {
            //Serial.println(arr[tt]); 
            Serial.print(tt,DEC);Serial.print(" ");Serial.print(*fString[tt]);Serial.print(" ");Serial.println(arr[tt]);
          }//Serial.println("Device");
          
          subscribing("printing");
          state = 1;
          break;
        case 1:
          parsing(payload);
                   for (tt = 0; tt < 9; tt++) //Iterate through results
          {
            //Serial.println(arr[tt]); 
            Serial.print(tt,DEC);Serial.print(" ");Serial.print(*fString[tt]);Serial.print(" ");Serial.println(arr[tt]);
          }//Serial.println("Device");
          
          subscribing("light", "up");
          
          state =0;
          break;
      }

}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();

}

void loop() {

 if (!client.connected()) {
  reconnect();
 }
   client.loop();
   yield();
 }
