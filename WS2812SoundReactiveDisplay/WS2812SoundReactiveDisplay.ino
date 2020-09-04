
//Please note this is a horribly hacked together example of 'just about' working code. If you think you can do better, you probably can.



//************
//Sound Level to light show sketch for the
//autogain microphone Amplifier from Adafruit on pin AO
//plus neopixel led functions (pin 6) mapped on to different sound levels to give music to light effects
//
//*************

//lines below set variables for neopixels

#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define NUMBER_PIXEL 32
#define LED_PIN     11
#define NUM_LEDS    1
#define LEDTYPE     WS2812B
#define COLOR_ORDER GRB
#define LED_HALF  NUM_LEDS/2
#define VISUALS   6   //Change this accordingly if you add/remove a visual in the switch-case in Visualize()
#define AUDIO_PIN A6  //Pin for the envelope of the sound detector
#define KNOB_PIN  A0  //Pin for the trimpot 10K
#define DC_OFFSET  1.25  // DC offset in mic signal - if unusure, leave 0
#define NOISE     260  // Noise/hum/interference in mic signal
#define SAMPLES   40  // Length of buffer for dynamic level adjustment
#define TOP       (NUMBER_PIXEL + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 60  // Rate of peak falling dot

const int buttonPin = 2;
int buttonState = 0;
int buttonPushCounter = 0;
int lastButtonState = 0;
int lng = 100;//long delay
int sht = 50;//short delay

byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 0,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;

CRGB leds[NUM_LEDS];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_PIXEL, LED_PIN, NEO_GRB + NEO_KHZ800);

//////////<Globals>

//  These values either need to be remembered from the last pass of loop() or
//  need to be accessed by several functions in one pass, so they need to be global.

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);  //LED strip objetcs

uint16_t gradient = 0; //Used to iterate and loop through each color palette gradually

//IMPORTANT:
//  This array holds the "threshold" of each color function (i.e. the largest number they take before repeating).
//  The values are in the same order as in ColorPalette()'s switch case (Rainbow() is first, etc). This is simply to
//  keep "gradient" from overflowing, the color functions themselves can take any positive value. For example, the
//  largest value Rainbow() takes before looping is 1529, so "gradient" should reset after 1529, as listed.
//     Make sure you add/remove values accordingly if you add/remove a color function in the switch-case in ColorPalette().
uint16_t thresholds[] = {1529, 1019, 764, 764, 764, 1274};

uint8_t palette = 0;  //Holds the current color palette.
uint8_t visual = 0;   //Holds the current visual being displayed.
uint8_t volume = 0;   //Holds the volume level read from the sound detector.
uint8_t last = 0;     //Holds the value of volume from the previous loop() pass.
uint8_t x = 0;
float maxVol = 5;    //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
float knob = 1023.0;  //Holds the percentage of how twisted the trimpot is. Used for adjusting the max brightness.
float avgBump = 0;    //Holds the "average" volume-change to trigger a "bump."
float avgVol = 0;     //Holds the "average" volume-level to proportionally adjust the visual experience.
float shuffleTime = 0;  //Holds how many seconds of runtime ago the last shuffle was (if shuffle mode is on).


//lines below are for the microphone sampling from Adafruit autogain mic

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//NOTE: The reason "average" is quoted is because it is not a true mathematical average. This is because I have
//      found what I call a "sequenced average" is more successful in execution than a real average. The difference
//      is that the sequenced average doesn't use the pool of all values recorded thus far, but rather averages the
//      last average and the current value received (in sequence). Concretely:
//
//          True average: (1 + 2 + 3) / 3 = 2
//          Sequenced: (1 + 2) / 2 = 1.5 --> (1.5 + 3) / 2 = 2.25  (if 1, 2, 3 was the order the values were received)
//
//      All "averages" in the program operate this way. The difference is subtle, but the reason is that sequenced
//      averages are more adaptive to changes in the overall volume. In other words, if you went from loud to quiet,
//      the sequenced average is more likely to show an accurate and proportional adjustment more fluently.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool shuffle = true;  //Toggles shuffle mode.
bool bump = false;     //Used to pass if there was a "bump" in volume

//For Traffic() visual
int8_t pos[NUM_LEDS] = { -2};    //Stores a population of color "dots" to iterate across the LED strip.
uint8_t rgb[NUM_LEDS][3] = {0};  //Stores each dot's specific RGB values.

//For Snake() visual
bool left = false;  //Determines the direction of iteration. Recycled in PaletteDance()
int8_t dotPos = 0;  //Holds which LED in the strip the dot is positioned at. Recycled in most other visuals.
float timeBump = 0; //Holds the time (in runtime seconds) the last "bump" occurred.
float avgTime = 0;  //Holds the "average" amount of time between each "bump" (used for pacing the dot's movement).

//////////</Globals>

void setup()
{
  strip.begin();//initialises neopixels
  strip.setBrightness(255);// set brightness from 0 to max is 255
  strip.show();//clears any previous data in the strip
  //strip.begin(); //Initialize the LED strip object.
  //strip.show();  //Show a blank strip, just to get the LED's ready for use.
  Serial.begin(9600);//set up for checking mic is working
   //Serial.println("mood FastLED WS2812B");
  //Serial.println("https://codebender.cc/sketch:220829");
  //delay(3000); //safety startup delay to protect LEDS
  memset(vol, 0, sizeof(vol));
  strip.begin();
  pinMode(buttonPin, INPUT);
  FastLED.addLeds<LEDTYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

}


void loop()
{
   buttonState = digitalRead(buttonPin);
   if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
      if (buttonPushCounter >= 5) {
        buttonPushCounter = 0;
      }
      delay(250);
      }
       lastButtonState = buttonState;
   }
       if (buttonPushCounter == 1)
       {

      static uint8_t hue = 170; // start at blue
  FastLED.showColor(CHSV(hue++, 255, 191)); // loop trough all the hue colors
  //FastLED.delay(300); // FastLED2.1 has a bug
  Serial.println(hue);
  delay(300);
    }

 else if (buttonPushCounter == 2)

{

//This is where the magic happens. This loop produces each frame of the visual.
//open void loop
  //first run the sound sampling
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    //open while loop
    sample = analogRead(AUDIO_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      //open 1st if loop in while
      if (sample > signalMax)
      {
        //open 2nd if
        signalMax = sample;  // save just the max levels
      }//close 2nd if
      else if (sample < signalMin)
      {
        //open 3rd if
        signalMin = sample;  // save just the min levels
      }//close 3rd if
    }//close 1st if
  }//close while loop
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
//Serial.println(volts);
 int sound = (volts * 10);
 //Serial.println(sound);
   volume = map(sound, 1, 10, 0, 255);
//Serial.println(volume);
  //Serial.println(volume);
//  knob = 1023.5 / 1023.0;//
knob = analogRead(KNOB_PIN) ;// 1023.0; //Record how far the trimpot is twisted

  //Sets a threshold for volume.
  //  In practice I've found noise can get up to 15, so if it's lower, the visual thinks it's silent.
  //  Also if the volume is less than average volume / 2 (essentially an average with 0), it's considered silent.
  if (volume < avgVol / 2.0 || volume < 15) volume = 0;

  else avgVol = (avgVol + volume) / 2.0; //If non-zeo, take an "average" of volumes.

  //If the current volume is larger than the loudest value recorded, overwrite
  if (volume > maxVol) maxVol = volume;

  //Check the Cycle* functions for specific instructions if you didn't include buttons in your design.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  CyclePalette();  //Changes palette for shuffle mode or button press.

  CycleVisual();   //Changes visualization for shuffle mode or button press.

  //ToggleShuffle(); //Toggles shuffle mode. Delete this if you didn't use buttons.
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  //This is where "gradient" is modulated to prevent overflow.
  if (gradient > thresholds[palette]) {
    gradient %= thresholds[palette] + 1;

    //Everytime a palette gets completed is a good time to readjust "maxVol," just in case
    //  the song gets quieter; we also don't want to lose brightness intensity permanently
    //  because of one stray loud sound.
    maxVol = (maxVol + volume) / 2.0;
  }

  //If there is a decent change in volume since the last pass, average it into "avgBump"
  if (volume - last > 10) avgBump = (avgBump + (volume - last)) / 2.0;

  //If there is a notable change in volume, trigger a "bump"
  //  avgbump is lowered just a little for comparing to make the visual slightly more sensitive to a beat.
  bump = (volume - last > avgBump * .9);  

  //If a "bump" is triggered, average the time between bumps
  if (bump) {
    avgTime = (((millis() / 1000.0) - timeBump) + avgTime) / 2.0;
    timeBump = millis() / 1000.0;
  }

  Visualize();   //Calls the appropriate visualization to be displayed with the globals as they are.

  gradient++;    //Increments gradient

  last = volume; //Records current volume for next pass

  delay(30);     //Paces visuals so they aren't too fast to be enjoyable


  
}

else if (buttonPushCounter == 3) 

{

  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;



  n   = analogRead(AUDIO_PIN);                  // Raw reading from mic
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top


  // Color pixels based on rainbow gradient
  for(i=0; i<NUMBER_PIXEL; i++) {
    if(i >= height)               strip.setPixelColor(i,   0,   0, 0);
    else strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
   
  }



  // Draw peak dot 
  if(peak > 0 && peak <= NUMBER_PIXEL-1) strip.setPixelColor(peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
 
   strip.show(); // Update strip

// Every few frames, make the peak pixel drop by 1:

    if(++dotCount >= PEAK_FALL) { //fall rate
     
      if(peak > 0) peak--;
      dotCount = 0;
    }



  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

else if (buttonPushCounter == 4)    

{rainbowCycle(3);//from the neopixel library
}
    else 
    
    
    {
  //open void loop
  //first run the sound sampling
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

    // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    //open while loop
    sample = analogRead(AUDIO_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      //open 1st if loop in while
      if (sample > signalMax)
      {
        //open 2nd if
        signalMax = sample;  // save just the max levels
      }//close 2nd if
      else if (sample < signalMin)
      {
        //open 3rd if
        signalMin = sample;  // save just the min levels
      }//close 3rd if
    }//close 1st if
  }//close while loop
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
//Serial.println(volts);
//section below maps the signal from the microphone on to 12 options for LED effects

  int sound = (volts * 10);
 //Serial.println(sound);
  int soundLevel = map(sound, 1, 10, 0, 11);
  //Serial.print("The volt level is  ");
 Serial.println(volts);//for debugging

//next section is a series of 12 (0-11) 'if' statements which trigger different patterns.
//it is a combination of a traditional VU style meter fill of the strip
// combined with randomised animated patterns to keep it interesting
  if (soundLevel == 0)
  {
    //open if 0. When there is silence a rainbow pattern runs

    rainbowCycle(3);//from the neopixel library

  }//close if 0 statement


  if (soundLevel == 1)
  {
    //open level 1 if statement which contains 4 randomised options

    int level1Color = random(1, 4);//choose random number 1 - 4

    if (level1Color == 1) //if random no 1 chosen light up pixels 1 and 2 red:
    {

      strip.setPixelColor(0, 255, 0, 0); // this turns on pixel 1 100% red (range runs 0 - 255) and leaves green and blue off
      strip.setPixelColor(1, 255, 0, 0); //  - you get the idea
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close red random no. 1

    else if (level1Color == 2) //if random no 2 choses show green
    {
      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close random no. 2 green

    else if (level1Color == 3) //run blue
    {

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close blue

    else if (level1Color == 4) //run yellow
    {

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close yellow



  }//end of if sound level 1 options


  if (soundLevel == 2)
  {
    //open level 2

    int level2Color = random(1, 5);//choose one of 5 options if sound level 2

    if (level2Color == 1) //run red mix
    {

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level2Color == 2) //run green mix
    {
      //open option 2

      strip.setPixelColor(0, 0, 206, 209);  
      strip.setPixelColor(1, 0, 206, 209);  
      strip.setPixelColor(2, 0, 206, 209);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close green

    else if (level2Color == 3) //run blue mix
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level2Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close yellow

    else if (level2Color == 5)//for a bit of variation 1 in 5 of level 2 will show a pattern across whole strip:
    {
      //open if 5
      strip.setPixelColor(0, 200, 75, 109);  
      strip.setPixelColor(1, 252, 203, 198);  
      strip.setPixelColor(2, 255, 216, 209);  
      strip.setPixelColor(3, 253, 215, 130);  
      strip.setPixelColor(4, 181, 198, 130);  
      strip.setPixelColor(5, 141, 189, 193);  
      strip.setPixelColor(6, 177, 217, 242);  
      strip.setPixelColor(7, 100, 165, 187);  
      strip.setPixelColor(8, 133, 105, 128);  
      strip.setPixelColor(9, 140, 166, 95);  
      strip.setPixelColor(10, 198, 44, 58);  
      strip.setPixelColor(11, 149, 69, 103);  


      strip.show();
      delay(lng);

      strip.setPixelColor(1, 200, 75, 109);  
      strip.setPixelColor(2, 252, 203, 198);  
      strip.setPixelColor(3, 255, 216, 209);  
      strip.setPixelColor(4, 253, 215, 130);  
      strip.setPixelColor(5, 181, 198, 130);  
      strip.setPixelColor(6, 141, 189, 193);  
      strip.setPixelColor(7, 177, 217, 242);  
      strip.setPixelColor(8, 100, 165, 187);  
      strip.setPixelColor(9, 133, 105, 128);  
      strip.setPixelColor(10, 140, 166, 95);  
      strip.setPixelColor(11, 198, 44, 58);  
      strip.setPixelColor(0, 149, 69, 103);  

      strip.show();
      delay(lng);

    }//close of option 5
  }//close level 2

  if (soundLevel == 3)
  {
    //open if sound level 3
    int level3Color = random(1, 5);

    if (level3Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level3Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 245, 116, 97);  
      strip.setPixelColor(1, 169, 221, 20);  
      strip.setPixelColor(2, 245, 116, 97);  
      strip.setPixelColor(3, 169, 221, 20);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2green

    else if (level3Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 169, 221, 199);  
      strip.setPixelColor(1, 245, 116, 97);  
      strip.setPixelColor(2, 169, 221, 199);  
      strip.setPixelColor(3,  245, 116, 97);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level3Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level3Color == 5)
    {
      //open option 5
      strip.setPixelColor(0, 255, 255, 255);  
      strip.setPixelColor(1, 255, 105, 180);  
      strip.setPixelColor(2, 255, 255, 255);  
      strip.setPixelColor(3, 255, 105, 180);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 255, 105, 180);  
      strip.setPixelColor(6, 255, 255, 255);  
      strip.setPixelColor(7, 255, 105, 180);  
      strip.setPixelColor(8, 255, 255, 255);  
      strip.setPixelColor(9, 255, 105, 180);  
      strip.setPixelColor(10, 255, 255, 255);  
      strip.setPixelColor(11, 255, 105, 180);  
      strip.show();
      delay(sht);
    }//close of option 5
  }//close level 3


  if (soundLevel == 4)
  {
    //open if sound level 4
    int level4Color = random(1, 5);

    if (level4Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close red

    else if (level4Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close green

    else if (level4Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 0, 255);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close blue

    else if (level4Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close yellow

    else if (level4Color == 5)
    {
      ////open option 5
      strip.setPixelColor(0, 255, 01, 165);  
      strip.setPixelColor(1, 255, 187, 218);  
      strip.setPixelColor(2, 228, 194, 191);  
      strip.setPixelColor(3, 153, 87, 205);  
      strip.setPixelColor(4, 176, 284, 218);  
      strip.setPixelColor(5, 67, 142, 200);  
      strip.setPixelColor(6, 107, 167, 214);  
      strip.setPixelColor(7, 168, 204, 232);  
      strip.setPixelColor(8, 59, 198, 182);  
      strip.setPixelColor(9, 100, 212, 199);  
      strip.setPixelColor(10, 164, 231, 223);  
      strip.setPixelColor(11, 176, 124, 218);  
      strip.show();
      delay(lng);
    }//close option 5

  }//close if sound level 4

  else if (soundLevel == 5)
  {
    //open if sound level 5


    int level5Color = random(1, 6);

    if (level5Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 255, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level5Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 255, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level5Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 0, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level5Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close yellow

    else if (level5Color == 5)
    {
      //open option 5

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 255);      
strip.setPixelColor(6, 0, 0, 255);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 0, 255, 0);  
      strip.setPixelColor(8, 0, 0, 255);  
      strip.setPixelColor(9, 255, 0, 0);  
      strip.setPixelColor(10, 0, 255, 0);  
      strip.setPixelColor(11, 0, 0, 255);  
      strip.show();
      delay(lng);
    }//close option 5

    else if (level5Color == 6)
    {
      //open option 6

      colorWipe(strip.Color(255, 0, 255), 50); // magenta
      colorWipe(strip.Color(0, 255, 0), 50); // green
      strip.show();
    }//close option 6
  }//close if sound level 5


  else if (soundLevel == 6)
  {
    //open if soundlevel 6

    int level6Color = random(1, 6);

    if (level6Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 255, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1red

    else if (level6Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 255, 0);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level6Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 0, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 0, 255);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level6Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 148, 0, 211);  
      strip.setPixelColor(1, 75, 0, 130);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 127, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(sht);
    }//close yellow

    else if (level6Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(0, 0, 255), 50); // Blue
      colorWipe(strip.Color(255, 255, 0), 50); // yellow
      strip.show();
    }//close option 5

    else if (level6Color == 6)
    {
      //open option6

      theaterChase(strip.Color(200, 0, 0), 50); // Red
      strip.show();
      delay(lng);
    }//close option 6
  }//close if sound level 6

  else if (soundLevel == 7)
  {
    //open if sound level 7
    int level7Color = random(1, 7);

    if (level7Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 255, 0, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 255, 0, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level7Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 255, 0);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 255, 0);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 255, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level7Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 0, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 0, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 0, 255);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level7Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 255, 255, 0);  
      strip.setPixelColor(7, 255, 255, 0);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level7Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 20, 147), 50); // pink
      colorWipe(strip.Color(0, 206, 209), 50); // turquoise
      strip.show();
      delay(lng);
    }//close option 5

    else if (level7Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(255, 20, 100), 50); // Red
      strip.show();
      delay(sht);
    }//close option 6

    else if (level7Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(1, 0, 100, 0);  
      strip.setPixelColor(2, 255, 0, 70);  
      strip.setPixelColor(3, 50, 0, 150);  
      strip.setPixelColor(4, 0, 70, 70);  
      strip.setPixelColor(5, 0, 100, 0);  
      strip.setPixelColor(6, 255, 0, 70);  
      strip.setPixelColor(7, 50, 0, 150);  
strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(8, 255, 0, 70);  
      strip.setPixelColor(9, 0, 100, 0);  
      strip.setPixelColor(10, 255, 0, 70);  
      strip.setPixelColor(11, 50, 0, 150);  

      strip.show();
      delay(sht);
    }//close option 7
  }//close if sound level 7

  else if (soundLevel == 8)
  {
    //open if sound level 8

    int level8Color = random(1, 8);

    if (level8Color == 1) //run red
    {
      //open option 1
      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 255, 0, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 255, 0, 0);  
      strip.setPixelColor(8, 255, 0, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level8Color == 2) //run green/blue
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 255, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green/blue

    else if (level8Color == 3) //run turquoise / blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 206, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 206, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 206, 255);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 206, 255);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level8Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 255, 255, 0);  
      strip.setPixelColor(7, 255, 255, 0);  
      strip.setPixelColor(8, 255, 255, 0);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level8Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 20, 147), 20); // pink
      colorWipe(strip.Color(0, 206, 209), 20); // turquoise
      strip.show();
    }//close option 5

    else if (level8Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(0, 206, 209), 50); // Red
      strip.show();
      delay(sht);
    }//close option 6

    else if (level8Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(1, 0, 100, 0);  
      strip.setPixelColor(2, 255, 0, 70);  
      strip.setPixelColor(3, 50, 0, 150);  
      strip.setPixelColor(4, 0, 70, 70);  
      strip.setPixelColor(5, 0, 100, 0);  
      strip.setPixelColor(6, 255, 0, 70);  
      strip.setPixelColor(7, 50, 0, 150);  
strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(8, 255, 0, 70);  
      strip.setPixelColor(9, 0, 100, 0);  
      strip.setPixelColor(10, 255, 0, 70);  
      strip.setPixelColor(11, 50, 0, 150);  

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);  
      strip.setPixelColor(1, 0, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 255, 255);  
      strip.setPixelColor(7, 0, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);


    }//close option 7

  }//close if sound level 8

  else if (soundLevel == 9)
  {
    //open if sound level 9

    int level9Color = random(1, 8);

    if (level9Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 255, 0, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 255, 0, 0);  
      strip.setPixelColor(8, 255, 0, 0);  
      strip.setPixelColor(9, 255, 0, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level9Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 255, 0);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level9Color == 3) //run blue
    {
      //open option 3
      strip.setPixelColor(0, 255, 0, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 255, 0, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 255, 0, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 255, 0, 255);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 255, 0, 255);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3blue

    else if (level9Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 255, 255, 0);  
      strip.setPixelColor(7, 255, 255, 0);  
      strip.setPixelColor(8, 255, 255, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level9Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 255, 255), 60); // white
      colorWipe(strip.Color(0, 206, 209), 20); // turquoise
      strip.show();
    }//close option 5

    else if (level9Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(50, 190, 209), 50); // turquise
      strip.show();
      delay(lng);
    }//close option 6

    else if (level9Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(1, 0, 100, 0);  
      strip.setPixelColor(2, 255, 0, 70);  
      strip.setPixelColor(3, 50, 0, 150);  
      strip.setPixelColor(4, 0, 70, 70);  
      strip.setPixelColor(5, 0, 100, 0);  
      strip.setPixelColor(6, 255, 0, 70);  
      strip.setPixelColor(7, 50, 0, 150);  
      strip.setPixelColor(8, 255, 0, 70);  
      strip.setPixelColor(9, 0, 100, 0);  
      strip.setPixelColor(10, 255, 0, 70);  
      strip.setPixelColor(11, 50, 0, 150);  

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);  
      strip.setPixelColor(1, 0, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 255, 255);  
      strip.setPixelColor(7, 0, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);

      strip.setPixelColor(0, 255, 50, 50);  
      strip.setPixelColor(1, 0, 255, 0);  
      strip.setPixelColor(2, 255, 50, 50);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 50, 50);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 255, 50, 50);  
      strip.setPixelColor(7, 0, 255, 0);  
      strip.setPixelColor(8, 255, 50, 50);  
      strip.setPixelColor(9, 255, 0, 0);  
      strip.setPixelColor(10, 255, 50, 50);  
      strip.setPixelColor(11, 0, 0, 255);  
      strip.show();

      delay(lng);


    }//close option 7

    else if (level9Color == 8)
    {
      //open option 8
      strip.setPixelColor(0, 255, 255, 255);  
      strip.setPixelColor(1, 0, 0, 0);  
      strip.setPixelColor(2, 255, 255, 255);  
      strip.setPixelColor(3, 0, 0, 0);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 255, 255, 255);  
      strip.setPixelColor(7, 0, 0, 0);  
      strip.setPixelColor(8, 255, 255, 255);  
      strip.setPixelColor(9, 0, 0, 0);  
      strip.setPixelColor(10, 255, 255, 255);  
      strip.setPixelColor(11, 0, 0, 0);  

      strip.show();
      delay (lng);

      strip.setPixelColor(0, 0, 0, 0);  
      strip.setPixelColor(1, 255, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 255);  
      strip.setPixelColor(4, 0, 0, 0);  
      strip.setPixelColor(5, 255, 255, 255);  
      strip.setPixelColor(6, 0, 0, 0);  
      strip.setPixelColor(7, 255, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 255);  
      strip.setPixelColor(10, 0, 0, 0);  
      strip.setPixelColor (11, 255, 255, 255);  

      strip.show();
      delay(lng);


    }//close option 9

  }//close if sound level 9

  else if (soundLevel == 10)

  {
    //open if sound Level 10

    int level10Color = random(1, 8);

    if (level10Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 255, 0, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 255, 0, 0);  
      strip.setPixelColor(8, 255, 0, 0);  
      strip.setPixelColor(9, 255, 0, 0);  
      strip.setPixelColor(10, 255, 0, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level10Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 255, 0);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level10Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 206, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 206, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 206, 255);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 206, 255);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 206, 255);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level10Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 255, 255, 0);  
      strip.setPixelColor(7, 255, 255, 0);  
      strip.setPixelColor(8, 255, 255, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level10Color == 5)
    {
      //open option 5

      colorWipe(strip.Color(200, 40, 147), 50); // pink
      colorWipe(strip.Color(0, 206, 209), 20); // turquoise
      strip.show();
      delay(sht);
    }//close option 5

    else if (level10Color == 6)
    {
      //open option 6
      theaterChase(strip.Color(0, 206, 209), 50);
      strip.show();
      delay(sht);
    }//close option 6

    else if (level10Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(1, 0, 100, 0);  
      strip.setPixelColor(2, 255, 0, 70);  
      strip.setPixelColor(3, 50, 0, 150);  
      strip.setPixelColor(4, 0, 70, 70);  
      strip.setPixelColor(5, 0, 100, 0);  
      strip.setPixelColor(6, 255, 0, 70);  
      strip.setPixelColor(7, 50, 0, 150);  
      strip.setPixelColor(8, 255, 0, 70);  
      strip.setPixelColor(9, 0, 100, 0);  
      strip.setPixelColor(10, 255, 0, 70);  
      strip.setPixelColor(11, 50, 0, 150);  

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);  
      strip.setPixelColor(1, 0, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 255, 255);  
      strip.setPixelColor(7, 0, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);

      strip.setPixelColor(0, 255, 255, 255);  
      strip.setPixelColor(1, 255, 255, 255);  
      strip.setPixelColor(2, 255, 255, 255);  
      strip.setPixelColor(3, 255, 255, 255);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 255, 255, 255);  
      strip.setPixelColor(6, 255, 255, 255);  
      strip.setPixelColor(7, 255, 255, 255);  
      strip.setPixelColor(8, 255, 255, 255);  
      strip.setPixelColor(9, 255, 255, 255);  
      strip.setPixelColor(10, 255, 255, 255);  
      strip.setPixelColor(11, 255, 255, 255);  
      strip.show();
      delay(sht);

    }//close option 7

  }//close if sound level 10


  else if (soundLevel == 11)

  {
    //open if sound Level 11

    int level11Color = random(1, 8);

    if (level11Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);  
      strip.setPixelColor(1, 255, 0, 0);  
      strip.setPixelColor(2, 255, 0, 0);  
      strip.setPixelColor(3, 255, 0, 0);  
      strip.setPixelColor(4, 255, 0, 0);  
      strip.setPixelColor(5, 255, 0, 0);  
      strip.setPixelColor(6, 255, 0, 0);  
      strip.setPixelColor(7, 255, 0, 0);  
      strip.setPixelColor(8, 255, 0, 0);  
      strip.setPixelColor(9, 255, 0, 0);  
      strip.setPixelColor(10, 255, 0, 0);  
      strip.setPixelColor(11, 255, 0, 0);  
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level11Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 255, 0);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 255, 0);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 255, 0);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 255, 0);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 255, 0);  
      strip.setPixelColor(11, 0, 0, 255);  
      strip.show();
      delay(lng);
    }//close option 2  green

    else if (level11Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);  
      strip.setPixelColor(1, 0, 0, 255);  
      strip.setPixelColor(2, 0, 206, 255);  
      strip.setPixelColor(3, 0, 0, 255);  
      strip.setPixelColor(4, 0, 206, 255);  
      strip.setPixelColor(5, 0, 0, 255);  
      strip.setPixelColor(6, 0, 206, 255);  
      strip.setPixelColor(7, 0, 0, 255);  
      strip.setPixelColor(8, 0, 206, 255);  
      strip.setPixelColor(9, 0, 0, 255);  
      strip.setPixelColor(10, 0, 206, 255);  
      strip.setPixelColor(11, 0, 0, 255);  
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level11Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 255, 255, 0);  
      strip.setPixelColor(1, 255, 255, 0);  
      strip.setPixelColor(2, 255, 255, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 255, 255, 0);  
      strip.setPixelColor(6, 255, 255, 0);  
      strip.setPixelColor(7, 255, 255, 0);  
      strip.setPixelColor(8, 255, 255, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 255, 255, 0);  
      strip.show();
      delay(lng);
    }//close option 4 yellow

    else if (level11Color == 5)
    {
      //open option 5

      colorWipe(strip.Color(0, 40, 255), 50); // pink
      colorWipe(strip.Color(0, 209, 206), 20); // turquoise
      strip.show();
      delay(sht);
    }//close option 5

    else if (level11Color == 6) //open option 6
    {
      //open option 6
      theaterChase(strip.Color(0, 206, 109), 50);
      strip.show();
      delay(sht);
    }//close option 6

    else if (level11Color == 7)//open option 7
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);  
      strip.setPixelColor(1, 0, 100, 0);  
      strip.setPixelColor(2, 255, 0, 70);  
      strip.setPixelColor(3, 50, 0, 150);  
      strip.setPixelColor(4, 0, 70, 70);  
      strip.setPixelColor(5, 0, 100, 0);  
      strip.setPixelColor(6, 255, 0, 70);  
      strip.setPixelColor(7, 50, 0, 150);  
      strip.setPixelColor(8, 255, 0, 70);  
      strip.setPixelColor(9, 0, 100, 0);  
      strip.setPixelColor(10, 255, 0, 70);  
      strip.setPixelColor(11, 50, 0, 150);  

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);  
      strip.setPixelColor(1, 0, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 255, 255);  
      strip.setPixelColor(7, 0, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);

      strip.setPixelColor(0, 255, 255, 255);  
      strip.setPixelColor(1, 255, 255, 255);  
      strip.setPixelColor(2, 255, 255, 255);  
      strip.setPixelColor(3, 255, 255, 255);  
      strip.setPixelColor(4, 255, 255, 255);  
      strip.setPixelColor(5, 255, 255, 255);  
      strip.setPixelColor(6, 255, 255, 255);  
      strip.setPixelColor(7, 255, 255, 255);  
      strip.setPixelColor(8, 255, 255, 255);  
      strip.setPixelColor(9, 255, 255, 255);  
      strip.setPixelColor(10, 255, 255, 255);  
      strip.setPixelColor(11, 255, 255, 255);  
      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);  
      strip.setPixelColor(1, 0, 255, 255);  
      strip.setPixelColor(2, 0, 0, 0);  
      strip.setPixelColor(3, 255, 255, 0);  
      strip.setPixelColor(4, 255, 255, 0);  
      strip.setPixelColor(5, 0, 0, 0);  
      strip.setPixelColor(6, 0, 255, 255);  
      strip.setPixelColor(7, 0, 255, 255);  
      strip.setPixelColor(8, 0, 0, 0);  
      strip.setPixelColor(9, 255, 255, 0);  
      strip.setPixelColor(10, 255, 255, 0);  
      strip.setPixelColor(11, 0, 0, 0);  
      strip.show();
      delay(lng);

    }//close option 7

  }//close if sound level 11

}//close void loop()
}
//neopixel functions below



void colorWipe(uint32_t c, uint8_t wait)
{
  //open colowipe
  for(uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}//close colorWipe function

void theaterChase(uint32_t c, uint8_t wait)
{
  //open theaterchase function
  for (int j = 0; j < 3; j++) //do 3 cycles of chasing
  {
    for (int q = 0; q < 3; q++)
    {
      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}//close theater chase function

void rainbowCycle(uint8_t wait)  //open rainbow function
{
  uint16_t i, j;

  for(j = 0; j < 256; j++) // 1 cycles of all colors on wheel
  {
    for(i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}//close rainbow function
//you need to included the code below for the neopixel functions that use the 'wheel' code:
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//end


//////////<Visual Functions>

//This function calls the appropriate visualization based on the value of "visual"
void Visualize() {
  switch (visual) {
    case 0: return Pulse(); Serial.println("Glitter");
    case 1: return PalettePulse();
    case 2: return Traffic();
    case 3: return Snake();
    case 4: return PaletteDance();
    case 5: return Glitter(); Serial.println("Glitter");
    case 6: return Paintball();
    default: return Pulse();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//NOTE: The strip displays RGB values as a 32 bit unsigned integer (uint32_t), which is why ColorPalette()
//      and all associated color functions' return types are uint32_t. This value is a composite of 3
//      unsigned 8bit integer (uint8_t) values (0-255 for each of red, blue, and green). You'll notice the
//      function split() (listed below) is used to dissect these 8bit values from the 32-bit color value.
//////////////////////////////////////////////////////////////////////////////////////////////////////////


//This function calls the appropriate color palette based on "palette"
//  If a negative value is passed, returns the appropriate palette withe "gradient" passed.
//  Otherwise returns the color palette with the passed value (useful for fitting a whole palette on the strip).
uint32_t ColorPalette(float num) {
  switch (palette) {
    case 0: return (num < 0) ? Rainbow(gradient) : Rainbow(num);
    case 1: return (num < 0) ? Sunset(gradient) : Sunset(num);
    case 2: return (num < 0) ? Ocean(gradient) : Ocean(num);
    case 3: return (num < 0) ? PinaColada(gradient) : PinaColada(num);
    case 4: return (num < 0) ? Sulfur(gradient) : Sulfur(num);
    case 5: return (num < 0) ? NoGreen(gradient) : NoGreen(num);
    default: return Rainbow(gradient);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//NOTE: All of these visualizations feature some aspect that affects brightness based on the volume relative to
//      maxVol, so that louder = brighter. Initially, I did simple proportions (volume/maxvol), but I found this
//      to be visually indistinct. I then tried an exponential method (raising the value to the power of
//      volume/maxvol). While this was more visually satisfying, I've opted for a balance between the two. You'll
//      notice something like pow(volume/maxVol, 2.0) in the functions below. This simply squares the ratio of
//      volume to maxVol to get a more exponential curve, but not as exaggerated as an actual exponential curve.
//      In essence, this makes louder volumes brighter, and lower volumes dimmer, to be more visually distinct.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//PULSE
//Pulse from center of the strip
void Pulse() {

  fade(0.75);   //Listed below, this function simply dims the colors a little bit each pass of loop()

  //Advances the palette to the next noticeable color if there is a "bump"
  if (bump) gradient += thresholds[palette] / 24;

  //If it's silent, we want the fade effect to take over, hence this if-statement
  if (volume > 0) {
    uint32_t col = ColorPalette(-1); //Our retrieved 32-bit color

    //These variables determine where to start and end the pulse since it starts from the middle of the strip.
    //  The quantities are stored in variables so they only have to be computed once (plus we use them in the loop).
    int start = LED_HALF - (LED_HALF * (volume / maxVol));
    int finish = LED_HALF + (LED_HALF * (volume / maxVol)) + strip.numPixels() % 2;
    //Listed above, LED_HALF is simply half the number of LEDs on your strip. ↑ this part adjusts for an odd quantity.

    for (int i = start; i < finish; i++) {

      //"damp" creates the fade effect of being dimmer the farther the pixel is from the center of the strip.
      //  It returns a value between 0 and 1 that peaks at 1 at the center of the strip and 0 at the ends.
      float damp = sin((i - start) * PI / float(finish - start));

      //Squaring damp creates more distinctive brightness.
      damp = pow(damp, 2.0);

      //Fetch the color at the current pixel so we can see if it's dim enough to overwrite.
      uint32_t col2 = strip.getPixelColor(i);

      //Takes advantage of one for loop to do the following:
      // Appropriatley adjust the brightness of this pixel using location, volume, and "knob"
      // Take the average RGB value of the intended color and the existing color, for comparison
      uint8_t colors[3];
      float avgCol = 0, avgCol2 = 0;
      for (int k = 0; k < 3; k++) {
        colors[k] = split(col, k) * damp * knob * pow(volume / maxVol, 2);
        avgCol += colors[k];
        avgCol2 += split(col2, k);
      }
      avgCol /= 3.0, avgCol2 /= 3.0;

      //Compare the average colors as "brightness". Only overwrite dim colors so the fade effect is more apparent.
      if (avgCol > avgCol2) strip.setPixelColor(i, strip.Color(colors[0], colors[1], colors[2]));
    }
  }
  //This command actually shows the lights. If you make a new visualization, don't forget this!
  strip.show();
}


//PALETTEPULSE
//Same as Pulse(), but colored the entire pallet instead of one solid color
void PalettePulse() {
  fade(0.75);
  if (bump) gradient += thresholds[palette] / 24;
  if (volume > 0) {
    int start = LED_HALF - (LED_HALF * (volume / maxVol));
    int finish = LED_HALF + (LED_HALF * (volume / maxVol)) + strip.numPixels() % 2;
    for (int i = start; i < finish; i++) {
      float damp = sin((i - start) * PI / float(finish - start));
      damp = pow(damp, 2.0);

      //This is the only difference from Pulse(). The color for each pixel isn't the same, but rather the
      //  entire gradient fitted to the spread of the pulse, with some shifting from "gradient".
      int val = thresholds[palette] * (i - start) / (finish - start);
      val += gradient;
      uint32_t col = ColorPalette(val);

      uint32_t col2 = strip.getPixelColor(i);
      uint8_t colors[3];
      float avgCol = 0, avgCol2 = 0;
      for (int k = 0; k < 3; k++) {
        colors[k] = split(col, k) * damp * knob * pow(volume / maxVol, 2);
        avgCol += colors[k];
        avgCol2 += split(col2, k);
      }
      avgCol /= 3.0, avgCol2 /= 3.0;
      if (avgCol > avgCol2) strip.setPixelColor(i, strip.Color(colors[0], colors[1], colors[2]));
    }
  }
  strip.show();
}


//TRAFFIC
//Dots racing into each other
void Traffic() {

  //fade() actually creates the trail behind each dot here, so it's important to include.
  fade(0.8);

  //Create a dot to be displayed if a bump is detected.
  if (bump) {

    //This mess simply checks if there is an open position (-2) in the pos[] array.
    int8_t slot = 0;
    for (slot; slot < sizeof(pos); slot++) {
      if (pos[slot] < -1) break;
      else if (slot + 1 >= sizeof(pos)) {
        slot = -3;
        break;
      }
    }

    //If there is an open slot, set it to an initial position on the strip.
    if (slot != -3) {

      //Evens go right, odds go left, so evens start at 0, odds at the largest position.
      pos[slot] = (slot % 2 == 0) ? -1 : strip.numPixels();

      //Give it a color based on the value of "gradient" during its birth.
      uint32_t col = ColorPalette(-1);
      gradient += thresholds[palette] / 24;
      for (int j = 0; j < 3; j++) {
        rgb[slot][j] = split(col, j);
      }
    }
  }

  //Again, if it's silent we want the colors to fade out.
  if (volume > 0) {

    //If there's sound, iterate each dot appropriately along the strip.
    for (int i = 0; i < sizeof(pos); i++) {

      //If a dot is -2, that means it's an open slot for another dot to take over eventually.
      if (pos[i] < -1) continue;

      //As above, evens go right (+1) and odds go left (-1)
      pos[i] += (i % 2) ? -1 : 1;

      //Odds will reach -2 by subtraction, but if an even dot goes beyond the LED strip, it'll be purged.
      if (pos[i] >= strip.numPixels()) pos[i] = -2;

      //Set the dot to its new position and respective color.
      //  I's old position's color will gradually fade out due to fade(), leaving a trail behind it.
      strip.setPixelColor( pos[i], strip.Color(
                              float(rgb[i][0]) * pow(volume / maxVol, 2.0) * knob,
                              float(rgb[i][1]) * pow(volume / maxVol, 2.0) * knob,
                              float(rgb[i][2]) * pow(volume / maxVol, 2.0) * knob)
                          );
    }
  }
  strip.show(); //Again, don't forget to actually show the lights!
}


//SNAKE
//Dot sweeping back and forth to the beat
void Snake() {
  if (bump) {

    //Change color a little on a bump
    gradient += thresholds[palette] / 30;

    //Change the direction the dot is going to create the illusion of "dancing."
    left = !left;
  }

  fade(0.975); //Leave a trail behind the dot.

  uint32_t col = ColorPalette(-1); //Get the color at current "gradient."

  //The dot should only be moved if there's sound happening.
  //  Otherwise if noise starts and it's been moving, it'll appear to teleport.
  if (volume > 0) {

    //Sets the dot to appropriate color and intensity
    strip.setPixelColor(dotPos, strip.Color(
                           float(split(col, 0)) * pow(volume / maxVol, 1.5) * knob,
                           float(split(col, 1)) * pow(volume / maxVol, 1.5) * knob,
                           float(split(col, 2)) * pow(volume / maxVol, 1.5) * knob)
                        );

    //This is where "avgTime" comes into play.
    //  That variable is the "average" amount of time between each "bump" detected.
    //  So we can use that to determine how quickly the dot should move so it matches the tempo of the music.
    //  The dot moving at normal loop speed is pretty quick, so it's the max speed if avgTime < 0.15 seconds.
    //  Slowing it down causes the color to update, but only change position every other amount of loops.
    if (avgTime < 0.15)                                               dotPos += (left) ? -1 : 1;
    else if (avgTime >= 0.15 && avgTime < 0.5 && gradient % 2 == 0)   dotPos += (left) ? -1 : 1;
    else if (avgTime >= 0.5 && avgTime < 1.0 && gradient % 3 == 0)    dotPos += (left) ? -1 : 1;
    else if (gradient % 4 == 0)                                       dotPos += (left) ? -1 : 1;
  }

  strip.show(); // Display the lights

  //Check if dot position is out of bounds.
  if (dotPos < 0)    dotPos = strip.numPixels() - 1;
  else if (dotPos >= strip.numPixels())  dotPos = 0;
}


//PALETTEDANCE
//Projects a whole palette which oscillates to the beat, similar to the snake but a whole gradient instead of a dot
void PaletteDance() {
  //This is the most calculation-intensive visual, which is why it doesn't need delayed.

  if (bump) left = !left; //Change direction of iteration on bump

  //Only show if there's sound.
  if (volume > avgVol) {

    //This next part is convoluted, here's a summary of what's happening:
    //  First, a sin wave function is introduced to change the brightness of all the pixels (stored in "sinVal")
    //      This is to make the dancing effect more obvious. The trick is to shift the sin wave with the color so it all appears
    //      to be the same object, one "hump" of color. "dotPos" is added here to achieve this effect.
    //  Second, the entire current palette is proportionally fitted to the length of the LED strip (stored in "val" each pixel).
    //      This is done by multiplying the ratio of position and the total amount of LEDs to the palette's threshold.
    //  Third, the palette is then "shifted" (what color is displayed where) by adding "dotPos."
    //      "dotPos" is added to the position before dividing, so it's a mathematical shift. However, "dotPos"'s range is not
    //      the same as the range of position values, so the function map() is used. It's basically a built in proportion adjuster.
    //  Lastly, it's all multiplied together to get the right color, and intensity, in the correct spot.
    //      "gradient" is also added to slowly shift the colors over time.
    for (int i = 0; i < strip.numPixels(); i++) {

      float sinVal = abs(sin(
                           (i + dotPos) *
                           (PI / float(strip.numPixels() / 1.25) )
                         ));
      sinVal *= sinVal;
      sinVal *= volume / maxVol;
      sinVal *= knob;

      unsigned int val = float(thresholds[palette] + 1)
                         //map takes a value between -NUM_LEDS and +NUM_LEDS and returns one between 0 and NUM_LEDS
                         * (float(i + map(dotPos, -1 * (strip.numPixels() - 1), strip.numPixels() - 1, 0, strip.numPixels() - 1))
                            / float(strip.numPixels()))
                         + (gradient);

      val %= thresholds[palette]; //make sure "val" is within range of the palette

      uint32_t col = ColorPalette(val); //get the color at "val"

      strip.setPixelColor(i, strip.Color(
                             float(split(col, 0))*sinVal,
                             float(split(col, 1))*sinVal,
                             float(split(col, 2))*sinVal)
                          );
    }

    //After all that, appropriately reposition "dotPos."
    dotPos += (left) ? -1 : 1;
  }

  //If there's no sound, fade.
  else  fade(0.8);

  strip.show(); //Show lights.

  //Loop "dotPos" if it goes out of bounds.
  if (dotPos < 0) dotPos = strip.numPixels() - strip.numPixels() / 6;
  else if (dotPos >= strip.numPixels() - strip.numPixels() / 6)  dotPos = 0;
}


//GLITTER
//Creates white sparkles on a color palette to the beat
void Glitter() {

  //This visual also fits a whole palette on the entire strip
  //  This just makes the palette cycle more quickly so it's more visually pleasing
  gradient += thresholds[palette] / 204;

  //"val" is used again as the proportional value to pass to ColorPalette() to fit the whole palette.
  for (int i = 0; i < strip.numPixels(); i++) {
    unsigned int val = float(thresholds[palette] + 1) *
                       (float(i) / float(strip.numPixels()))
                       + (gradient);
    val %= thresholds[palette];
    uint32_t  col = ColorPalette(val);

    //We want the sparkles to be obvious, so we dim the background color.
    strip.setPixelColor(i, strip.Color(
                           split(col, 0) / 6.0 * knob,
                           split(col, 1) / 6.0 * knob,
                           split(col, 2) / 6.0 * knob)
                        );
  }

  //Create sparkles every bump
  if (bump) {

    //Random generator needs a seed, and micros() gives a large range of values.
    //  micros() is the amount of microseconds since the program started running.
    randomSeed(micros());

    //Pick a random spot on the strip.
    dotPos = random(strip.numPixels() - 1);

    //Draw  sparkle at the random position, with appropriate brightness.
    strip.setPixelColor(dotPos, strip.Color(
                           255.0 * pow(volume / maxVol, 2.0) * knob,
                           255.0 * pow(volume / maxVol, 2.0) * knob,
                           255.0 * pow(volume / maxVol, 2.0) * knob
                         ));
  }
  bleed(dotPos);
  strip.show(); //Show the lights.
}


//PAINTBALL
//Recycles Glitter()'s random positioning; simulates "paintballs" of
//  color splattering randomly on the strip and bleeding together.
void Paintball() {

  //If it's been twice the average time for a "bump" since the last "bump," start fading.
  if ((millis() / 1000.0) - timeBump > avgTime * 2.0) fade(0.99);

  //Bleeds colors together. Operates similarly to fade. For more info, see its definition below
  bleed(dotPos);

  //Create a new paintball if there's a bump (like the sparkles in Glitter())
  if (bump) {

    //Random generator needs a seed, and micros() gives a large range of values.
    //  micros() is the amount of microseconds since the program started running.
    randomSeed(micros());

    //Pick a random spot on the strip. Random was already reseeded above, so no real need to do it again.
    dotPos = random(strip.numPixels() - 1);

    //Grab a random color from our palette.
    uint32_t col = ColorPalette(random(thresholds[palette]));

    //Array to hold final RGB values
    uint8_t colors[3];

    //Relates brightness of the color to the relative volume and potentiometer value.
    for (int i = 0; i < 3; i++) colors[i] = split(col, i) * pow(volume / maxVol, 2.0) * knob;

    //Splatters the "paintball" on the random position.
    strip.setPixelColor(dotPos, strip.Color(colors[0], colors[1], colors[2]));

    //This next part places a less bright version of the same color next to the left and right of the
    //  original position, so that the bleed effect is stronger and the colors are more vibrant.
    for (int i = 0; i < 3; i++) colors[i] *= .8;
    strip.setPixelColor(dotPos - 1, strip.Color(colors[0], colors[1], colors[2]));
    strip.setPixelColor(dotPos + 1, strip.Color(colors[0], colors[1], colors[2]));
  }
  strip.show(); //Show lights.
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//DEBUG CYCLE
//No reaction to sound, merely to see gradient progression of color palettes
//NOT implemented in code as is, but is easily includable in the switch-case.
void Cycle() {
  for (int i = 0; i < strip.numPixels(); i++) {
    float val = float(thresholds[palette]) * (float(i) / float(strip.numPixels())) + (gradient);
    val = int(val) % thresholds[palette];
    strip.setPixelColor(i, ColorPalette(val));
  }
  strip.show();
  gradient += 32;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////</Visual Functions>


//////////<Helper Functions>

void CyclePalette() {

  //IMPORTANT: Delete this whole if-block if you didn't use buttons//////////////////////////////////
//
//  //If a button is pushed, it sends a "false" reading
//  if (!digitalRead(BUTTON_1)) {
//
//    palette++;     //This is this button's purpose, to change the color palette.
//
//    //If palette is larger than the population of thresholds[], start back at 0
//    //  This is why it's important you add a threshold to the array if you add a
//    //  palette, or the program will cylce back to Rainbow() before reaching it.
//    if (palette >= sizeof(thresholds) / 2) palette = 0;
//
//    gradient %= thresholds[palette]; //Modulate gradient to prevent any overflow that may occur.
//
//    //The button is close to the microphone on my setup, so the sound of pushing it is
//    //  relatively loud to the sound detector. This causes the visual to think a loud noise
//    //  happened, so the delay simply allows the sound of the button to pass unabated.
//    delay(350);
//
//    maxVol = avgVol;  //Set max volume to average for a fresh experience.
//  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  //If shuffle mode is on, and it's been 30 seconds since the last shuffle, and then a modulo
  //  of gradient to get a random decision between palette or visualization shuffle
  if (shuffle && millis() / 1000.0 - shuffleTime > 30 && gradient % 2) {

    shuffleTime = millis() / 1000.0; //Record the time this shuffle happened.

    palette++;
    if (palette >= sizeof(thresholds) / 2) palette = 0;
    gradient %= thresholds[palette];
    maxVol = avgVol;  //Set the max volume to average for a fresh experience.
  }
}


void CycleVisual() {

  //IMPORTANT: Delete this whole if-block if you didn't use buttons//////////////////////////////////
//  if (!digitalRead(BUTTON_2)) {
//
//    visual++;     //The purpose of this button: change the visual mode
//
//    gradient = 0; //Prevent overflow
//
//    //Resets "visual" if there are no more visuals to cycle through.
//    if (visual > VISUALS) visual = 0;
//    //This is why you should change "VISUALS" if you add a visual, or the program loop over it.
//
//    //Resets the positions of all dots to nonexistent (-2) if you cycle to the Traffic() visual.
//    if (visual == 1) memset(pos, -2, sizeof(pos));
//
//    //Gives Snake() and PaletteDance() visuals a random starting point if cycled to.
//    if (visual == 2 || visual == 3) {
//      randomSeed(analogRead(0));
//      dotPos = random(strip.numPixels());
//    }
//
//    //Like before, this delay is to prevent a button press from affecting "maxVol."
//    delay(350);
//
//    maxVol = avgVol; //Set max volume to average for a fresh experience
//  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  //If shuffle mode is on, and it's been 30 seconds since the last shuffle, and then a modulo
  //  of gradient WITH INVERTED LOGIC to get a random decision between what to shuffle.
  //  This guarantees one and only one of these shuffles will occur.
  if (shuffle && millis() / 1000.0 - shuffleTime > 10 && !(gradient % 2)) {

    shuffleTime = millis() / 1000.0; //Record the time this shuffle happened.

    visual++;
    gradient = 0;
    if (visual > VISUALS) visual = 0;
    if (visual == 1) memset(pos, -2, sizeof(pos));
    if (visual == 2 || visual == 3) {
      randomSeed(volume);
      dotPos = random(strip.numPixels());
    }
    maxVol = avgVol;
  }
}


//IMPORTANT: Delete this function  if you didn't use buttons./////////////////////////////////////////
//void ToggleShuffle() {
//  if (!digitalRead(BUTTON_3)) {
//
//    shuffle = !shuffle; //This button's purpose: toggle shuffle mode.
//
//    //This delay is to prevent the button from taking another reading while you're pressing it
//    delay(500);
//
//    //Reset these things for a fresh experience.
//    maxVol = avgVol;
//    avgBump = 0;
//  }
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//Fades lights by multiplying them by a value between 0 and 1 each pass of loop().
void fade(float damper) {

  //"damper" must be between 0 and 1, or else you'll end up brightening the lights or doing nothing.

  for (int i = 0; i < strip.numPixels(); i++) {

    //Retrieve the color at the current position.
    uint32_t col = strip.getPixelColor(i);

    //If it's black, you can't fade that any further.
    if (col == 0) continue;

    float colors[3]; //Array of the three RGB values

    //Multiply each value by "damper"
    for (int j = 0; j < 3; j++) colors[j] = split(col, j) * damper;

    //Set the dampened colors back to their spot.
    strip.setPixelColor(i, strip.Color(colors[0] , colors[1], colors[2]));
  }
}


//"Bleeds" colors currently in the strip by averaging from a designated "Point"
void bleed(uint8_t Point) {
  for (int i = 1; i < strip.numPixels(); i++) {

    //Starts by look at the pixels left and right of "Point"
    //  then slowly works its way out
    int sides[] = {Point - i, Point + i};

    for (int i = 0; i < 2; i++) {

      //For each of Point+i and Point-i, the pixels to the left and right, plus themselves, are averaged together.
      //  Basically, it's setting one pixel to the average of it and its neighbors, starting on the left and right
      //  of the starting "Point," and moves to the ends of the strip
      int point = sides[i];
      uint32_t colors[] = {strip.getPixelColor(point - 1), strip.getPixelColor(point), strip.getPixelColor(point + 1)  };

      //Sets the new average values to just the central point, not the left and right points.
      strip.setPixelColor(point, strip.Color(
                             float( split(colors[0], 0) + split(colors[1], 0) + split(colors[2], 0) ) / 3.0,
                             float( split(colors[0], 1) + split(colors[1], 1) + split(colors[2], 1) ) / 3.0,
                             float( split(colors[0], 2) + split(colors[1], 2) + split(colors[2], 2) ) / 3.0)
                          );
    }
  }
}


//As mentioned above, split() gives you one 8-bit color value
//from the composite 32-bit value that the NeoPixel deals with.
//This is accomplished with the right bit shift operator, ">>"
uint8_t split(uint32_t color, uint8_t i ) {

  //0 = Red, 1 = Green, 2 = Blue

  if (i == 0) return color >> 16;
  if (i == 1) return color >> 8;
  if (i == 2) return color >> 0;
  return -1;
}

//////////</Helper Functions>


//////////<Palette Functions>

//These functions simply take a value and return a gradient color
//  in the form of an unsigned 32-bit integer

//The gradients return a different, changing color for each multiple of 255
//  This is because the max value of any of the 3 RGB values is 255, so it's
//  an intuitive cutoff for the next color to start appearing.
//  Gradients should also loop back to their starting color so there's no jumps in color.

uint32_t Rainbow(unsigned int i) {
  if (i > 1529) return Rainbow(i % 1530);
  if (i > 1274) return strip.Color(255, 0, 255 - (i % 255));   //violet -> red
  if (i > 1019) return strip.Color((i % 255), 0, 255);         //blue -> violet
  if (i > 764) return strip.Color(0, 255 - (i % 255), 255);    //aqua -> blue
  if (i > 509) return strip.Color(0, 255, (i % 255));          //green -> aqua
  if (i > 255) return strip.Color(255 - (i % 255), 255, 0);    //yellow -> green
  return strip.Color(255, i, 0);                               //red -> yellow
}

uint32_t Sunset(unsigned int i) {
  if (i > 1019) return Sunset(i % 1020);
  if (i > 764) return strip.Color((i % 255), 0, 255 - (i % 255));          //blue -> red
  if (i > 509) return strip.Color(255 - (i % 255), 0, 255);                //purple -> blue
  if (i > 255) return strip.Color(255, 128 - (i % 255) / 2, (i % 255));    //orange -> purple
  return strip.Color(255, i / 2, 0);                                       //red -> orange
}

uint32_t Ocean(unsigned int i) {
  if (i > 764) return Ocean(i % 765);
  if (i > 509) return strip.Color(0, i % 255, 255 - (i % 255));  //blue -> green
  if (i > 255) return strip.Color(0, 255 - (i % 255), 255);      //aqua -> blue
  return strip.Color(0, 255, i);                                 //green -> aqua
}

uint32_t PinaColada(unsigned int i) {
  if (i > 764) return PinaColada(i % 765);
  if (i > 509) return strip.Color(255 - (i % 255) / 2, (i % 255) / 2, (i % 255) / 2);  //red -> half white
  if (i > 255) return strip.Color(255, 255 - (i % 255), 0);                            //yellow -> red
  return strip.Color(128 + (i / 2), 128 + (i / 2), 128 - i / 2);                       //half white -> yellow
}

uint32_t Sulfur(unsigned int i) {
  if (i > 764) return Sulfur(i % 765);
  if (i > 509) return strip.Color(i % 255, 255, 255 - (i % 255));   //aqua -> yellow
  if (i > 255) return strip.Color(0, 255, i % 255);                 //green -> aqua
  return strip.Color(255 - i, 255, 0);                              //yellow -> green
}

uint32_t NoGreen(unsigned int i) {
  if (i > 1274) return NoGreen(i % 1275);
  if (i > 1019) return strip.Color(255, 0, 255 - (i % 255));         //violet -> red
  if (i > 764) return strip.Color((i % 255), 0, 255);                //blue -> violet
  if (i > 509) return strip.Color(0, 255 - (i % 255), 255);          //aqua -> blue
  if (i > 255) return strip.Color(255 - (i % 255), 255, i % 255);    //yellow -> aqua
  return strip.Color(255, i, 0);                                     //red -> yellow
}

//NOTE: This is an example of a non-gradient palette: you will get straight red, white, or blue
//      This works fine, but there is no gradient effect, this was merely included as an example.
//      If you wish to include it, put it in the switch-case in ColorPalette() and add its
//      threshold (764) to thresholds[] at the top.
uint32_t USA(unsigned int i) {
  if (i > 764) return USA(i % 765);
  if (i > 509) return strip.Color(0, 0, 255);      //blue
  if (i > 255) return strip.Color(128, 128, 128);  //white
  return strip.Color(255, 0, 0);                   //red
}


//////////</Palette Functions>

//// Input a value 0 to 255 to get a color value.
//// The colors are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//  if(WheelPos < 85) {
//   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//  } else if(WheelPos < 170) {
//   WheelPos -= 85;
//   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  } else {
//   WheelPos -= 170;
//   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//}
