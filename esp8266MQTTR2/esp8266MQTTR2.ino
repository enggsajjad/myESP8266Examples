
//https://appcodelabs.com/introduction-to-iot-how-to-build-a-simple-mqtt-subscriber-using-esp8266

/*
Testing in Terminal 1:
c:\Program Files (x86)\mosquitto>mosquitto_sub.exe -t "test/message"
hello from ESP8266
on
off

Other Terminal 2:

c:\Program Files (x86)\mosquitto>mosquitto_pub -t "test/message" -m "on"

c:\Program Files (x86)\mosquitto>mosquitto_pub -t "test/message" -m "off"

c:\Program Files (x86)\mosquitto>
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//Add a couple of macro definitions for convenience:-
// use onboard LED for convenience 
#define LED (5)
// maximum received message length 
#define MAX_MSG_LEN (128)
//Now you need the WiFi config:-
// Wifi configuration
const char* ssid = "UPC0870375";
const char* password = "Ghnfdd6byuxp";
//And now the MQTT configuration:-
// MQTT Configuration
// if you have a hostname set for the MQTT server, you can use it here
//const char *serverHostname = "your MQTT server hostname";
// otherwise you can use an IP address like this
//const IPAddress serverIPAddress(192, 168, 0, 11);
const IPAddress serverIPAddress(192,168,0,164);
// the topic we want to use
const char *topic = "test/message";
//Create the MQTT and WiFi stacks:-
WiFiClient espClient;
PubSubClient client(espClient);
//Setup Function
//Here’s the familiar Arduino setup function:-
void setup() {
  // LED pin as output
  pinMode(LED, OUTPUT);      
  digitalWrite(LED, HIGH);
  // Configure serial port for debugging
  Serial.begin(115200);
  // Initialise wifi connection - this will wait until connected
  connectWifi();
  // connect to MQTT server  
  //client.setServer(serverHostname, 1883);
  client.setServer(serverIPAddress, 1883);
  client.setCallback(callback);
}
//First of all the LED is configured as an output and turned off: logic high means off because the ESP9266 on-board LED is wired with inverse logic. You’ll use the LED later to show the status of MQTT messages.

//Then the serial port is set up, and the WiFi configured.

//Finally, the client sets the MQTT server address and the callback function. The callback function is explained below.

//No attempt is made to connect to the server just yet.

//Main Loop
//The main loop is fairly straightforward:-

void loop() {
    if (!client.connected()) {
      connectMQTT();
    }
    // this is ESSENTIAL!
    client.loop();
    // idle
    delay(500);
}
//The first thing the main loop does it check if the MQTT server is connected. If not, it calls the connectMQTT() function, which will wait until a connection is established. It’s important for the MQTT server to be running at this point.

//Next, the MQTT client’s own loop() function is called. This is a vital step and no messages will be received is this is not called repeatedly from the main loop.

//Finally the code waits a short time to save power and CPU like a good citizen.

//Connecting to WiFi
//The function to connect to WiFi is fairly straightforward:-

// connect to wifi
void connectWifi() {
  delay(10);
  // Connecting to a WiFi network
  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP address ");
  Serial.println(WiFi.localIP());
}
//It begins the connection using WiFi.begin(ssid, password) and simply waits forever until it’s connected.

//Establishing an MQTT Connection
//The connectMQTT() function simply checks for a connection, and of none is active it attempts to connect to the MQTT server.

// connect to MQTT server
void connectMQTT() {
  // Wait until we're connected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish(topic, "hello from ESP8266");
      // ... and resubscribe
      client.subscribe(topic);
    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);
    }
  }
}
/*
 * A random client name is chosen for each connection attempt in order to prevent name clashes. The crucial piece of code is client.connect(clientId.c_str()) which attempts to establish the connection.

If successful, the client publishes a “hello” message on the topic test/message (defined globally at the top of the file). It then subscribes to the topic.

The Message Callback Function
One of the most important functions is the message callback function. If you remember in the setup() function, there was a line to register the callback function:-

  // ... inside setup() ...
  client.setCallback(callback);
A callback is a function that will get called from elsewhere in the code when something important happens. Often it is used by third-party library code so it can inform user code when some event is detected, such as data being received.

That is the case here. Because your user code has no knowledge of how the PubSubClient code works, you can’t edit the library code directly to make it do something when an MQTT message is received. So you register a callback, that is, you give the PubSubClient library the name of a function you want it to call when a message is received.

Because of the way C works, the callback must confirm to a predefined prototype, in this case:-

void (*callback)(char *msgTopic, byte *msgPayload, unsigned int msgLength);
This was defined by the creator of the PubSubClient library. It’s like a contractual agreement: the library agrees to inform you when a message is received, but its terms are that you must write a function that conforms to this prototype in order to receive the message.

If you’ve programmed languages that use interfaces or protocols then the concept is similar, but older and more primitive.

It can be called whatever you want, so long as the name is passed to client.setCallback(...). For simplicity it’s just called callback here. Similarly, you can name the arguments whatever you like so long as they are of the correct type and in the correct order.

OK, enough talk. Here’s the callback function:-
*/

void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength) {
  // copy payload to a static string
  static char message[MAX_MSG_LEN+1];
  if (msgLength > MAX_MSG_LEN) {
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char *)msgPayload, msgLength);
  message[msgLength] = '\0';
  
  Serial.printf("topic %s, message received: %s\n", topic, message);

  // decode message
  if (strcmp(message, "off") == 0) {
    setLedState(false);
  } else if (strcmp(message, "on") == 0) {
    setLedState(true);
  }
}
/*The first thing it does is takes a copy of the message, which is then null-terminated to make it a C string. It’s important in this case because the code is going to interpret the message as a string, but there’s nothing in the MQTT protocol that says messages have to be strings: they are simply sequences of bytes, which could be interpreted in any way the application intended.

The array where the message string is stored is statically allocated just once, on the heap. The allocated space is that of the maximum message size the function accepts, plus one for the string-terminating NULL byte.

The final piece of the function just checks the message string. If it’s equal to “on” then the LED is turned on, and if it’s “off” then the LED is turned off. The helper function setLedState() takes care of the hardware details:-
*/
void setLedState(boolean state) {
  // LED logic is inverted, low means on
  digitalWrite(LED, !state);
}
/*Does the Callback Have to Be Re-entrant?
You might have questioned the callback code for allocating the message string statically rather than creating a new buffer on the stack every time the function is called. What if the function were called again before the first invocation was complete?

If you did question it, then kudos to you, but your misgivings are unfounded. The callback does not have to be re-entrant, that is, it will not be called again before it has finished executing. For that reason it is safe (in execution terms) to use a static buffer to handling the incoming messages.

Because of the nature of the Arduino core’s simple scheduling system, all user code is executed from  the main loop and all incoming messages are properly buffered.

In embedded systems it’s very common to use a static buffer in this situation. There are several reasons for this, but they usually come down to limited RAM and the desire to create deterministic code.

Non-deterministic code, such as when the stack gets too big, is often a source of difficult-to-find bugs. Keeping large buffer allocations off the stack helps prevent uncontrollable stack growth and reduces this kind of bug.
*/
/*Testing the Client
Ensure you have another computer, virtual machine or Raspberry Pi with the mosquitto-client package installed. (It can be the same one the MQTT broker is running on)

Load the code onto the ESP8266 board and open the Arduino’s Serial Monitor to check it connects to the wifi and the MQTT server OK.

You should see something like this:-

Connecting to NETGEAR-AP
...

WiFi connected on IP address 192.168.0.21

MQTT connecting as client ESP8266-12ca...
MQTT connected
Now from the mosquitto-client machine, launch an terminal and type:-

mosquitto_pub -h mqtt-server-hostname -t "test/message" -m "on"
where mqtt-server-hostname is the hostname of your MQTT server. (Alternatively you can use its IP address.)

When you run this command, the following message should appear in the Arduino Serial Monitor:-

topic test/message, message received: on
And more importantly, the on-board LED should light up!

Send this message:-

mosquitto_pub -h mqtt-server-hostname -t "test/message" -m "off"
And the LED should now turn off.
*/
