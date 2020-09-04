// AVR Timer CTC Interrupts Calculator
// v. 8
// http://www.arduinoslovakia.eu/application/timer-calculator
// Microcontroller: ATmega328P
// Created: 2019-06-11T12:04:49.212Z

#define ledPin 13

void setupTimer2() {
  noInterrupts();
  // Clear registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // 250 Hz (16000000/((249+1)*256))
  OCR2A = 249;
  // CTC
  TCCR2A |= (1 << WGM21);
  // Prescaler 256
  TCCR2B |= (1 << CS22) | (1 << CS21);
  // Output Compare Match A Interrupt Enable
  TIMSK2 |= (1 << OCIE2A);
  interrupts();
}

void setup() {
  pinMode(ledPin, OUTPUT);
  setupTimer2();
}

void loop() {
}

ISR(TIMER2_COMPA_vect) {
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
}
