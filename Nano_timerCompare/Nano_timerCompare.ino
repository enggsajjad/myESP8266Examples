/* Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example 
   more infos: http://www.letmakerobots.com/node/28278
   created by RobotFreak 
Blinking LED with compare match interrupt

The first example uses the timer1 in CTC mode and the compare match interrupt to toggle a LED. The timer is configured for a frequency of 2Hz. The LED is toggled in the interrupt service routine.


*/
#define ledPin 13


void setup()

{

  pinMode(ledPin, OUTPUT);

  

  // initialize timer1 

  noInterrupts();           // disable all interrupts

  TCCR1A = 0;

  TCCR1B = 0;

  TCNT1  = 0;


  OCR1A = 31250;            // compare match register 16MHz/256/2Hz

  TCCR1B |= (1 << WGM12);   // CTC mode

  TCCR1B |= (1 << CS12);    // 256 prescaler 

  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

  interrupts();             // enable all interrupts

}


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine

{

  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin

}


void loop()

{

  // your program here…

}
