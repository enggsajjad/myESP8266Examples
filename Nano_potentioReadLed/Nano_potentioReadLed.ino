/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/
const int analogPin = A0;//the analog input pin attach to
const int ledPin = 9;//the led attach to
int inputValue = 0;//variable to store the value coming from sensor
int outputValue = 0;//variable to store the output value

#define SENSITIVITY 16
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int potentio = analogRead(A0);
  int a  = potentio/SENSITIVITY;
  int pSensitiyed = a*SENSITIVITY;
  // print out the value you read:
  analogWrite(ledPin,pSensitiyed);//turn the led on depend on the output value

  Serial.println(pSensitiyed);
  delay(100);        // delay in between reads for stability
}
