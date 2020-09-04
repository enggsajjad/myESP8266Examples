/*
 * 
 https://techtutorialsx.com/2017/04/29/esp32-sending-json-messages-over-mqtt/
Testing:

c:\Program Files (x86)\mosquitto>mosquitto_sub.exe -t "esp/test" -u "Sajjad" -P "#sajjan2"
{"device":"ESP32","sensorType":"Temperature","values":[20,21,23]}
{"device":"ESP32","sensorType":"Temperature","values":[20,21,23]}
*/


#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
 
//const char* ssid = "UPC0870375";
//const char* password =  "Ghnfdd6byuxp";
const char* ssid     = "tecoLecture";
const char* password = "PerComSS16";
//const char* mqttServer = "m11.cloudmqtt.com";
const char* mqttServer = "broker.mqtt-dashboard.com";
//const char* mqttServer = "iot.eclipse.org";
//const int mqttPort = 12948;
const IPAddress serverIPAddress(192,168,0,164);

const int mqttPort = 1883;
const char* mqttUser = "Sajjad";
const char* mqttPassword = "#sajjan2";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  //or
  //client.setServer(serverIPAddress, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    //if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
    if (client.connect("ESP32Client" )) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
}
 
void loop() {
 
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Temperature";
  JsonArray& values = JSONencoder.createNestedArray("values");

  //for(int k=0;k<256;k++)
  //  values.add(k);
  values.add(20);
  values.add(21);
  values.add(23);
 
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
 
  if (client.publish("/esp/test", JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
 
  client.loop();
  Serial.println("-------------");
 
  delay(10000);
 
}
