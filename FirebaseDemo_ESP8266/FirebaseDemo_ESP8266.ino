
#include <PulseSensorPlayground.h>
#include <FirebaseESP32.h>
#include <FirebaseArduino.h>
//#include <FirebaseCloudMessaging.h>
//#include <FirebaseError.h>
//#include <FirebaseHttpClient.h>
//#include <FirebaseObject.h>

#include <Adafruit_Sensor.h>

#include <DHT.h>

//#include <FirebaseArduino.h>
#include <WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define WIFI_SSID "Dhan GURU NANAK"
#define WIFI_PASSWORD "090078601"

#define FIREBASE_HOST "smartkit-startr.firebaseio.com"
#define FIREBASE_AUTH "JcYg8QyG2iuMVju9OKPaCarAvMxrayfpsDujSaf8"


#define ONE_WIRE_BUS 4   //D2 pin of nodemcu
#define DHTPIN 5

int PulseSensorPurplePin = A0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED13 = 16;   //  The on-board Arduion LED
int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;            // Determine which Signal to "count as a beat", and which to ingore.

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

OneWire oneWire(ONE_WIRE_BUS);
 
DallasTemperature sensors(&oneWire);            // Pass the oneWire reference to Dallas Temperature.

void setup(void)
{
  Serial.begin(115200); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WIFI not connecting...");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println(FIREBASE_HOST);
  pinMode(LED13,OUTPUT);
  sensors.begin();
  dht.begin();
}

void loop(void)
{ 

  delay(30000);
  sensors.requestTemperatures();                // Send the command to get temperatures  
  Serial.println("Celsius Temperature: ");
  float celsTemp = sensors.getTempCByIndex(0);
  Serial.println(celsTemp); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

  float celsTemp1;

  if(celsTemp < 0)
  {
    celsTemp1 = 0;
  }
  else
  {
   celsTemp1 = celsTemp;
  }

  String celsiusTemp = String(celsTemp1)+String("Â°C");
  
  Serial.println("Fahrenheit temperature: ");
  float farhTemp = sensors.getTempFByIndex(0);  
  Serial.println(farhTemp);
  float farhTemp1;

  if(farhTemp < 0)
  {
    farhTemp1 = 0;
  }
  else
  {
   farhTemp1 = farhTemp;
  }

  
  String farhientTemp = String(farhTemp1)+String("Â°F");

  Firebase.setInt("/bodytemp1",celsTemp1);
  Firebase.setInt("/bodytemp2",farhTemp1);

  Firebase.pushString("/patient/7Njc7GpJD2WXqoR5vQEjvQhco2b2/sensorsData/bodytemperature",farhientTemp);


  float h = dht.readHumidity();
  float t = dht.readTemperature();  

  if(isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read DHT Sensor!"));
    return;
    }

   Serial.print("Humidity"); Serial.print(h);
   String firehumid = String(h)+String("%");

   Serial.print("% Temperature: "); Serial.print(t); Serial.println("Â°C");
   String fireTemp = String(t)+String("Â°C");

   Firebase.setInt("/Humidity",h);
   Firebase.setInt("/RoomTemperature",t);

   Firebase.pushString("/patient/7Njc7GpJD2WXqoR5vQEjvQhco2b2/sensorsData/humidity",firehumid);
   Firebase.pushString("/patient/7Njc7GpJD2WXqoR5vQEjvQhco2b2/sensorsData/roomtemperature",fireTemp);

  Signal = analogRead(PulseSensorPurplePin); 
  int signal1 = Signal/6;  
  int signal2 = (Signal/6)-5;// Read the PulseSensor's value.
                                              // Assign this value to the "Signal" variable.

  Serial.println(signal1);   
  Serial.println(signal2);// Send the Signal value to Serial Plotter.


   if(Signal > Threshold){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
     digitalWrite(LED13,HIGH);
   } else {
     digitalWrite(LED13,LOW);                //  Else, the sigal must be below "550", so "turn-off" this LED.
   }
   String firebeat = String(signal1)+String("bpm");
   String firepulse = String(signal2)+String("ppm");

   Firebase.setInt("/Heartbeat",signal1);
   Firebase.setInt("/PulseRate",signal2);

   //Firebase.pushString("/SensorsData/Heartbeat",firebeat);
   Firebase.pushString("/patient/7Njc7GpJD2WXqoR5vQEjvQhco2b2/sensorsData/pulserate",firepulse);

   
}
