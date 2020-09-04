#define HALT while(true);
#define STATE_NORMAL 0
#define STATE_SHORT 1
#define STATE_LONG 2

const int LED = 13;

// Button input related values
static const byte BUTTON_PIN = 2;
volatile int  resultButton = 0; // global value set by checkButton()
static boolean edgeTriggered = false;
static boolean holding = false;
static boolean clicked = false;
char state1 = 0;
char buttonStaus=0; //0=Unpressed, 1=Clicked, 2=Hold
void setup() {

  
  noInterrupts();           // disable all interrupts

  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println(F("Initializing Button pin"));
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), checkButton, CHANGE);
  Serial.println(F("Button pin initialized"));
  /////////////////
  // initialize timer1 
  /*TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 31250*2;            // compare match register 16MHz/256/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  //TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  */
  TIMSK1 &= (0 << OCIE1A);
  TCCR1B &= (0 << WGM12);
  interrupts();             // enable all interrupts
}

void loop() {
  //while (true) {
  
    if(holding==true)
    {
      digitalWrite(LED, 1);//operation while holding
    }else{
      digitalWrite(LED, 0);//operation while holding off
    }
/*
    if(clicked==true)
    {
      
      digitalWrite(LED, 1);//operation while holding
      delay(1000);
      digitalWrite(LED, 0);//operation while holding off
    }*/
    
    if(edgeTriggered==true)
    {
      switch(state1)
      {
        case 0:
          
          delay(40);
          edgeTriggered=false;
          Serial.println(F("--------------------"));
          Serial.println(F("Button Falling edge"));
          
          TCCR1A = 0;  TCCR1B = 0;   TCNT1  = 0;   OCR1A = 31250*2;
          TCCR1B |= (1 << WGM12);   // CTC mode
          TCCR1B |= (1 << CS12);    // 256 prescaler
          TIMSK1 |= (1 << OCIE1A);
          
          clicked=false;          
          state1=1;

          break;
        case 1:
           TCCR1A = 0;  TCCR1B = 0;   TCNT1  = 0;   OCR1A = 31250*2;
          TIMSK1 &= (0 << OCIE1A);
          
          delay(40);
          edgeTriggered=false;
          Serial.println(F("Button Rising edge"));
          
          if(holding==false)
          {
            Serial.println(F("Button Clicked"));
            clicked=true;
          }
          holding=false;
          

          state1=0;
          break;
        case 2:
          break;
        }
    }//edgeTriggered
}


//*****************************************************************
void checkButton() {
  if(edgeTriggered==false)
  {
    edgeTriggered=true;
    //Serial.println(F("Button edge"));
  }
}


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  TIMSK1 &= (0 << OCIE1A);
  holding=true;
  //digitalWrite(LED, digitalRead(LED) ^ 1);   // toggle LED pin
  Serial.println(F("Button Pressed and Hold"));
}
