/*
  Serial Event example

  When new serial data arrives, this sketch adds it to a String.
  When a newline is received, the loop prints the string and clears it.

  A good test for this is to try it with a GPS receiver that sends out
  NMEA 0183 sentences.

  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.

  created 9 May 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialEvent
*/

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool motorstatse = false;  // whether the string is complete
//static const byte MOTOR = 7;
const int LED = 5;
void setup() {
  // initialize serial:
  delay(1000);
  Serial.begin(115200);
  Serial.println("LED TEST");
  //pinMode(MOTOR,OUTPUT);
  //digitalWrite(MOTOR, LOW);
    pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);
  // reserve 200 bytes for the inputString:
  //inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
 /* if (stringComplete) {
    Serial.println(inputString);
    Serial.println("Done!!");
    // clear the string:
    inputString = "";
    stringComplete = false;
    //digitalWrite(MOTOR, motorstatse);
    //digitalWrite(LED, motorstatse);
    //motorstatse = ! motorstatse;
  }*/
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    char newChar = inChar + 1;
    // add it to the inputString:
    inputString += inChar;
    Serial.print(newChar);
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    //if (inChar == '\n') {
      //stringComplete = true;
    //}
        if (inChar == '0' )
    {

      Serial.println("LED ON");
      digitalWrite(5,HIGH);
    }
    else if (inChar == '1')
    {
      Serial.println("LED OFF");
      digitalWrite(5,LOW);
    }
  }
}
