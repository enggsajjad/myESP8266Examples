#include "ArduinoJson.h"
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
}
 
void loop() {
 
  Serial.println("—————— -");
  char JSONMessage[] = " {\"device\": \"none\", \"mapping\": [\"SAJJAD\", \"HUSSAIN\", \"BHATTI\",\"SAJJAD1\", \"HUSSAIN1\", \"BHATTI1\",\"SAJJAD2\", \"HUSSAIN2\"]}";
  //char JSONMessage[] = " {\"device\": \"none\", \"mapping\": [\"SAJJAD\", \"HUSSAIN\", \"BHATTI\"]}";
  Serial.print("Message to parse: ");
  Serial.println(JSONMessage);
 
  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(JSONMessage);   //Parse message
 
  if (!parsed.success()) {      //Check for errors in parsing
 
    Serial.println("Parsing failed");
    delay(5000);
    return;
 
  }
 
  const char * device = parsed["device"]; //Get sensor type value
  Serial.print("device type: ");
  Serial.println(device);
 
  int arraySize = parsed["mapping"].size();   //get size of JSON Array
  Serial.print("\nSize of mapping array: ");
  Serial.println(arraySize);

  String arr[arraySize];
  Serial.println("\nArray mappings without explicit casting");
  for (int i = 0; i < arraySize; i++) 
  { //Iterate through results
    String mappingValue = parsed["mapping"][i];  //Implicit cast
    arr[i] = mappingValue;
    //Serial.println(mappingValue);
  }
  for (int i = 0; i < arraySize; i++) //Iterate through results
    Serial.println(arr[i]); 

  Serial.println();
  delay(10000);
 
}
