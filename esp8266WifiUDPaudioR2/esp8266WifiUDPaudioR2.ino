#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid     = "tecoLecture";
//const char* password = "PerComSS16";
const char* ssid     = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
int contconexion = 0;

WiFiUDP Udp;

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

}
unsigned char buf0[40960];
unsigned int cnt;
unsigned char state;
void loop()
{
  cnt=0;
  Serial.println("Start Speaking");
  delay(2000);
  
  for(int i=0; i<(1024*40);i++)
  {
    int old=micros();
    float analog1 = analogRead(A0);
    unsigned char analog = map(analog1,0,1023,0,255);
    buf0[cnt++] = analog;
    while(micros()-old<122);
    yield();
  }
  Serial.println("Start Sending");
  for(int j=0; j<40;j++)
  {
    Udp.beginPacket("192.168.0.164", 1234);//my room
    // Udp.beginPacket("192.168.10.121", 1234);//teco
    for(int i=0; i<1024;i++)
    {
      Udp.write(buf0[1024*j+i]);
      //yield();
    }
    Udp.endPacket();
   delay(10);
  }
}
