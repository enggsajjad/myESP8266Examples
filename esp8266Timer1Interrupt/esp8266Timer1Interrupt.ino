/*
    ESP8266 Timer Example
    Hardware: NodeMCU
    Circuits4you.com
    2018
    LED Blinking using Timer
*/
#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker blinker;

#define LED 5  //On board LED

//=======================================================================
void ICACHE_RAM_ATTR onTimerISR(){
    digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    //timer1_write(600000);//120000 us
    timer1_write(5000000); //1s
    //timer1_write(2500000);
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("");

    pinMode(LED,OUTPUT);


    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);//80MHz/16=5MHz
    //timer1_enable(TIM_DIV8, TIM_EDGE, TIM_SINGLE);//80MHz/32=2.5MHz
    //timer1_write(600000); //120000 us
    timer1_write(5000000); //1s=.02us x 5000000
    //timer1_write(2500000); //1s=.4us x 2500000
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
}
//=======================================================================
