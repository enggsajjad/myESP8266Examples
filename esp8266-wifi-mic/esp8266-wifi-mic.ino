
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char *ssid =  "SSID";	// Change it
//const char *password =  "PASS";	// Change it
const char* ssid     = "UPC0870375";
const char* password = "Ghnfdd6byuxp";

WiFiClient wclient;
//IPAddress mqtt_server(192,168,0,10);		// Change it
IPAddress mqtt_server(192,168,0,164);

//PubSubClient client(wclient, mqtt_server);
PubSubClient client(wclient);
//	client name
String str_name="sajjad";

String str_topic = String("/" + str_name + "/out/rec");
//String str_payload;
char str_payload[11000];

bool bt=true;
byte rec;
int count;
//char stream[24000];

void setup()
{
	Serial.begin(115200);
	Serial.println(0);			//start
	pinMode(15,INPUT_PULLUP);  //  bt D15
  pinMode(5,OUTPUT);      //  rele D5 led

	WiFi.mode(WIFI_STA);
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

  //
    //client.setServer(mqtt_server, 1883);
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop()
{
	my_connect();
	if(client.connected())
	{
		//my_button();
		my_analog();
	}

}

void my_connect()
{
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("1");			//Connecting to
		WiFi.begin(ssid, password);

		if(WiFi.waitForConnectResult() != WL_CONNECTED)
		{
			Serial.println("2");		//WiFi NOT connected
			if(digitalRead(5)==LOW)
				digitalWrite(5,HIGH);
			return;
		}
		Serial.println("3");			//WiFi connected
	}
	if(WiFi.status() == WL_CONNECTED)
	{
		if(!client.connected())
		{
			Serial.println("4");				//Connecting to MQTT server
			if(client.connect(str_name.c_str()))
			{
				Serial.println("5");				//Connected to MQTT server
			}
			else
			{
				Serial.println("6");		//Could not connect to MQTT server
				if(digitalRead(5)==LOW)
					digitalWrite(5,HIGH);
			}
		}
		if(client.connected())
		{
			client.loop();
			if(digitalRead(5)==HIGH)
				digitalWrite(5,LOW);
		}
	}
}

void my_print()
{
	//Serial.println(millis());
	//client.publish(str_topic,str_payload);
  client.publish("/sajjad/out/rec",str_payload);
	//str_payload="";
	//Serial.println(millis());
}

void my_button()
{
	if(digitalRead(15)==LOW && count<15)
		count++;
	if(digitalRead(15)==HIGH && count>0)
		count--;
	if(count>10 && bt==false)
 {
		bt=true;
    //digitalWrite(5,HIGH);
 }
	if(count<5 && bt==true)
 {
		bt=false;
   //digitalWrite(5,LOW);
 }
}

void my_analog()
{
	if(bt==true)
	{
		//Serial.println(millis());
		my_record();
		//Serial.println(millis());
	}
}

void my_record()
{
	bt=false;
	for(int y=0;y<11;y++)
	{
		for(int i=0;i<1000;i++)
		{
			//rec = char(analogRead(A0)/4);
			//if(rec<200)
				//str_payload += rec;
			//str_payload += char(analogRead(A0)/4);
      str_payload[1000*y+i]= char(analogRead(A0)/4);
			//delayMicroseconds(25);	//125-8kb
		}
	my_print();
	}
}
