/* 
 * Arduino 101: timer and interrupts
 * 2: Timer1 overflow interrupt example 
 * more infos: http://www.letmakerobots.com/node/28278
 * created by RobotFreak 

 Blinking LED with compare match interrupt

The first example uses the timer1 in CTC mode and the compare match interrupt to toggle a LED. The timer is configured for a frequency of 2Hz. The LED is toggled in the interrupt service routine.

same example like before but now we use the timer overflow interrupt. In this case timer1 is running in normal mode. 

The timer must be preloaded every time in the interrupt service routine.
 */

#define ledPin 13

void setup()
{
  pinMode(ledPin, OUTPUT);

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 34286;            // preload timer 65536-16MHz/256/2Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  TCNT1 = 34286;            // preload timer
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
}

void loop()
{
  // your program here...
}
