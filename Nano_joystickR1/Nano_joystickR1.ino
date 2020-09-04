#define JOYSTICK_SENSITIVITY 10
int timeContextLastPress, timeContextKeyPress;
char dir=1;
void setup() {
  Serial.begin(115200);
}
 
void loop() {
  int x = analogRead(A0);
  int y = analogRead(A1);
  timeContextKeyPress = millis();
  if ( timeContextKeyPress - timeContextLastPress >= 1000)
  {
    timeContextLastPress = timeContextKeyPress;
  Serial.print("The X and Y coordinates are:");
  Serial.print(x, DEC);
  Serial.print(",");
  Serial.println(y, DEC);
  Serial.println(" ");
  switch(dir)
  {
    case 1:
      Serial.print("Right\n");
      break;
    case 2:
      Serial.print("Left\n");
      break;
    case 3:
      Serial.print("Down\n");
      break;
    case 4:
      Serial.print("Up\n");
      break;
    case 5:
      Serial.print("Middle\n");
      break;
  }
  }
    //X
  if ((x<384) && (y<640) && (y>384))
    dir = 1;
  else if ((x>640) && (y<640) && (y>384))
    dir = 2;
    //Y
  else   if ((y<384) && (x<640) && (x>384))
    dir = 3;
  else if ((y>640) && (x<640) && (x>384))
    dir = 4;
  else
    dir = 5;

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
