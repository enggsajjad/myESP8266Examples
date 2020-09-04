#define JOYSTICK_SENSITIVITY 500
void setup() {
  Serial.begin(115200);
}
 
void loop() {
  int x = analogRead(A0);
  int y = analogRead(A1);
  Serial.print("The X and Y coordinates are:");
  Serial.print(x, DEC);
  Serial.print(",");
  Serial.println(y, DEC);
  Serial.println(" ");
    //X
  if ((x<384) && (y<640) && (y>384))
    Serial.print("Right\n");
  else if ((x>640) && (y<640) && (y>384))
    Serial.print("Left\n");
    //Y
  else   if ((y<384) && (x<640) && (x>384))
    Serial.print("Down\n");
  else if ((y>640) && (x<640) && (x>384))
    Serial.print("Up\n");
  else
    Serial.print("Middle\n");
/*if (y > 500 ){
  Serial.print("Up\n");
  }
  else if(y < 200){
   Serial.print("Down\n"); 
    }
   else if(x < 200){
    Serial.print("Left\n");
    }
   else if(x > 500){
    Serial.print("Right\n");
    }
    */
  delay(JOYSTICK_SENSITIVITY);
}
