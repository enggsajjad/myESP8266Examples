#define HALT while(true);
#define STATE_NORMAL 0
#define STATE_SHORT 1
#define STATE_LONG 2

const int LED = 13;

// Button input related values
static const byte BUTTON_PIN = 2;
volatile int  resultButton = 0; // global value set by checkButton()
static boolean flag1 = false;
char buttonState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println(F("Initializing Button pin"));
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), checkButton, CHANGE);
  Serial.println(F("Button pin initialized"));
}

void loop() {
  //while (true) {
    if(flag1==true)
    {
      
      
      switch(buttonState)
      {
        case 0:
          //detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
          delay(100);
          flag1=false;
          //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), checkButton, RISING);
          Serial.println(F("Button Falling edge"));
          buttonState=1;
          break;
        case 1:
          //detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
          delay(100);
          flag1=false;
          //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), checkButton, FALLING);
          Serial.println(F("Button Rising edge"));
          buttonState=0;
          break;
        case 2:
          break;
        }

    }//flag1
  //}
}


//*****************************************************************
void checkButton() {
  if(flag1==false){
  flag1=true;
  Serial.println(F("Button edge"));
  }
}
