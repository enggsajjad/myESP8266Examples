/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

  Testing
  run manually as administrator
  c:\Program Files (x86)\mosquitto>mosquitto
  c:\Program Files (x86)\mosquitto>mosquitto_sub.exe
  c:\Program Files (x86)\mosquitto>mosquitto_pub.exe
  c:\Program Files (x86)\mosquitto>mosquitto_sub.exe -t "outTopic"
hello world #34
hello world #35
hello world #36

C:\Windows\system32>cd "c:\Program Files (x86)\mosquitto"

c:\Program Files (x86)\mosquitto>mosquitto_pub -t "inTopic" -m "HELLO WORLD!"

In Arduino IDE Serial Terminal
..
Message arrived [inTopic] HELLO WORLD!
Publish message: hello world #244
..

*/

//code write by Moz for YouTube changel logMaker360, 24-11-2016
//code belongs to this video: https://youtu.be/nAUUdbUkJEI

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid = "UPC0870375";                           //!!!!!!!!!!!!!!!!!!!!!
const char* password = "Ghnfdd6byuxp";                //!!!!!!!!!!!!!!!!!!!!!
const char* mqtt_server = "192.168.137.51/24"; //laptop 192.168.0.164
//const char* mqtt_server = "192.168.0.164";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
const byte ledPin = 5; // digital pin 4 on a weMos D1 mini is next to ground so easy to stick a LED in.
 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '1')
  digitalWrite(ledPin, HIGH);
  if (receivedChar == '0')
   digitalWrite(ledPin, LOW);
  }
  Serial.println();
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("ledStatus");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}
 
void setup()
{
 Serial.begin(115200);
 
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 
 pinMode(ledPin, OUTPUT);
 digitalWrite(ledPin, HIGH);
 delay(5000);
 digitalWrite(ledPin, LOW);
}
 
void loop()
{
 if (!client.connected()) {
  reconnect();
 }
 client.loop();
}

/* Python code LEDblink.py
import paho.mqtt.publish as publish
import time

While True:
   print("Sending 1...")
   publish.single("ledStatus", "1", hostname="Your broker IP")
   time.sleep(6)
   print("Sending 0...")
   publish.single("ledStatus", "0", hostname="Your broker IP")
   time.sleep(3)

*/
