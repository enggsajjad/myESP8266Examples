//ESP8266 as MQTT Client – Arduino IoT
//https://electrosome.com/esp8266-mqtt-client-arduino-iot/


#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char *ssid = "UPC0870375";//"SERVER NAME"; 
const char *password = "Ghnfdd6byuxp";//"SERVER PASSWORD"; 
const char *mqtt_server = "broker.mqtt-dashboard.com";//"m24.cloudmqtt.com";//"m11.cloudmqtt.com"; 
const char *device_id = "esp8266";

WiFiClient espClient;
PubSubClient client(espClient);

const byte ledPin5 = 5;
char message_buff[100];

void callback(char *led_control, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(led_control);
  Serial.println("] ");
  int i;
  for (i = 0; i < length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  Serial.println(msgString);
  if (strcmp(led_control, "esp8266/led_control") == 0)
  { 
    if (msgString == "1")
    {
      digitalWrite(ledPin5, LOW); // PIN HIGH will switch OFF the relay
    }
    if (msgString == "0")
    {
      digitalWrite(ledPin5, HIGH); // PIN LOW will switch ON the relay
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(device_id, "rjvywkal", "JtBQnGZkbhxm"))
    { 
    Serial.println("connected");
    client.subscribe("esp8266/led_control"); // write your unique ID here
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  client.setServer(mqtt_server, 1883);//10777); // change port number as mentioned in your cloudmqtt console
  client.setCallback(callback);

  pinMode(ledPin5, OUTPUT);
  digitalWrite(ledPin5, LOW);
  client.publish("esp8266/led_control", "Hello Sajjad!");
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
