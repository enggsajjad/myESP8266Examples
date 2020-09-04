#define HALT while(true);
#define STATE_NORMAL 0
#define STATE_SHORT 1
#define STATE_LONG 2

const int LED = 13;

// Button input related values
static const byte BUTTON_PIN = 2;
//static const int  STATE_NORMAL = 0; // no button activity
//static const int  STATE_SHORT  = 1; // short button press
//static const int  STATE_LONG  = 2; // long button press
volatile int  resultButton = 0; // global value set by checkButton()

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
  int longButton=0;
  int count=0;
 
  while (true) {
  switch (resultButton) {

  case STATE_NORMAL: {
/*  Serial.print(".");
  count++;
  count = count % 10;
  if (count==0) Serial.println(""); */
  break;
  }

  case STATE_SHORT: {
  Serial.println("Short press has been detected");
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
  resultButton=STATE_NORMAL;
  break;
  }

  case STATE_LONG: {
  Serial.println("Button was pressed for long time");
  digitalWrite(LED,HIGH);
  delay(3000);
  digitalWrite(LED,LOW);
  longButton++;
  resultButton=STATE_NORMAL;
  break;
  }
  }
  if (longButton==5) {
  Serial.println("Halting");
  HALT;
  }
 }
}


//*****************************************************************
void checkButton() {
  /*
  * This function implements software debouncing for a two-state button.
  * It responds to a short press and a long press and identifies between
  * the two states. Your sketch can continue processing while the button
  * function is driven by pin changes.
  */

  const unsigned long LONG_DELTA = 3000ul;               // hold seconds for a long press
  const unsigned long DEBOUNCE_DELTA = 30ul;        // debounce time
  static int lastButtonStatus = HIGH;                                   // HIGH indicates the button is NOT pressed
  int buttonStatus;                                                                    // button atate Pressed/LOW; Open/HIGH
  static unsigned long longTime = 0ul, shortTime = 0ul; // future times to determine is button has been poressed a short or long time
  boolean Released = true, Transition = false;                  // various button states
  boolean timeoutShort = false, timeoutLong = false;    // flags for the state of the presses

  buttonStatus = digitalRead(BUTTON_PIN);                // read the button state on the pin "BUTTON_PIN"
  timeoutShort = (millis() > shortTime);                          // calculate the current time states for the button presses
  timeoutLong = (millis() > longTime);

  if (buttonStatus != lastButtonStatus) {                          // reset the timeouts if the button state changed
      shortTime = millis() + DEBOUNCE_DELTA;
      longTime = millis() + LONG_DELTA;
  }

  Transition = (buttonStatus != lastButtonStatus);        // has the button changed state
  Released = (Transition && (buttonStatus == HIGH)); // for input pullup circuit

  lastButtonStatus = buttonStatus;                                     // save the button status

  if ( ! Transition) {                                                                //without a transition, there's no change in input
  // if there has not been a transition, don't change the previous result
       resultButton =  STATE_NORMAL | resultButton;
       return;
  }

  if (timeoutLong && Released) {                                      // long timeout has occurred and the button was just released
       resultButton = STATE_LONG | resultButton;       // ensure the button result reflects a long press
  } else if (timeoutShort && Released) {                          // short timeout has occurred (and not long timeout) and button was just released
      resultButton = STATE_SHORT | resultButton;     // ensure the button result reflects a short press
  } else {                                                                                  // else there is no change in status, return the normal state
      resultButton = STATE_NORMAL | resultButton; // with no change in status, ensure no change in button status
  }
}
