#include "ArduinoJson.h"
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
}
 
void loop() {
 
  Serial.println("—————— -");
  char JSONMessage[] = " {\"SensorType\": \"Temperature\", \"Value\": [\"SAJJAD\", \"HUSSAIN\", \"BHATTI\"]}";
  Serial.print("Message to parse: ");
  Serial.println(JSONMessage);
 
  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(JSONMessage);   //Parse message
 
  if (!parsed.success()) {      //Check for errors in parsing
 
    Serial.println("Parsing failed");
    delay(5000);
    return;
 
  }
 
  const char * sensorType = parsed["SensorType"]; //Get sensor type value
  Serial.print("Sensor type: ");
  Serial.println(sensorType);
 
  int arraySize = parsed["Value"].size();   //get size of JSON Array
  Serial.print("\nSize of value array: ");
  Serial.println(arraySize);
 
  Serial.println("\nArray values without explicit casting");
  for (int i = 0; i < arraySize; i++) { //Iterate through results
 
    String sensorValue = parsed["Value"][i];  //Implicit cast
    Serial.println(sensorValue);
 
  }
 
  Serial.println("\nArray values with explicit casting");
  for (int i = 0; i < arraySize; i++) {  //Iterate through results
 
    Serial.println(parsed["Value"][i].as<int>());//Explicit cast
 
  }
 
  Serial.println();
  delay(10000);
 
}
