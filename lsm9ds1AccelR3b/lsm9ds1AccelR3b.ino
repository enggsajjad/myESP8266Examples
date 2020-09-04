#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
//#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <Ticker.h>  //Ticker Library

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

unsigned char gState =0,cntms=0,curDirection;

const int numReadings = 25;
const int numAxis =3;
float readings[numAxis][numReadings];  // the reading history
int readIndex[numAxis];              // the index of the current reading
float total[numAxis];                  // the running total
float average[numAxis];                // the average
void readLSM(void);
Ticker tmr(readLSM, 25, 0, MILLIS);
float axm, aym, azm;
  sensors_event_t a, m, g, temp;
  
void setupSensor()
{
  // 1.) Set the accelerometer range
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  ///lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  ///lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}


void setup() 
{
  delay(100);
  Serial.begin(115200);

  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  Serial.println("LSM9DS1 data read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    //while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
  setupSensor();

    // zero-fill all the arrays:
  for (int axis = 0; axis < numAxis; axis++) {
    readIndex[axis] = 0;
    total[axis] = 0;
    average[axis] = 0;
    for (int i = 0; i<numReadings; i++){
        readings[axis][i] = 0;
    }
  }
  //tmr.start();
}
unsigned long previous25Millis;
unsigned long previous3000Millis;
void loop() 
{
  yield();
  unsigned long currentMillis = millis();
  if ((currentMillis - previous25Millis) >= 25) 
  {
    previous25Millis = currentMillis;
    //25ms expired
    lsm.read();  /* ask it to read in the data */ 
    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp); 
    //tmr.update();
    //acceleration m/s^2
    axm = a.acceleration.x;//map(a.acceleration.x,-16*9.8,16*9.8,-32768,32767);
    aym = a.acceleration.y;//map(a.acceleration.y,-16*9.8,16*9.8,-32768,32767);
    azm = a.acceleration.z;//map(a.acceleration.z,-16*9.8,16*9.8,-32768,32767);
    //smooth(Ax, a.acceleration.x);
    //smooth(Ay, a.acceleration.y);
    //smooth(Az, a.acceleration.z);  
    smooth(0, axm);
    smooth(1, aym);
    smooth(2, azm);

  /*
    cntms++;
    
    switch(gState)
    {
      case 0:
        if(average[0]<-0.8)
        {
          curDirection = 1;//left
          //Serial.println("One");
        }
        if(average[0]>0.7)
        {
          curDirection = 2;//right
          //Serial.println("Two");
        }
        if(average[2]<-0.2)
        {
          curDirection = 3;//away
          //Serial.println("Three");
        }
        if(average[2]>1.3)
        {
          curDirection = 4;//closer
          //Serial.println("Four");
        }
        if (curDirection>0) gState = 1;
        break;
      case 1:
        if(cntms==40)
        {
          curDirection = 0;
          gState = 0;
          cntms = 0;
          Serial.println("none");
        }
        else
        {
          switch(curDirection)
          {
            case 1: if(average[0]>0.5) {Serial.println("left");curDirection = 0; gState = 0; cntms = 0;} break;
            case 2: if(average[0]<-0.6)  {Serial.println("right");curDirection = 0; gState = 0; cntms = 0;} break;
            case 3: if(average[2]>1.3)  {Serial.println("away");curDirection = 0; gState = 0; cntms = 0;} break;
            case 4: if(average[2]<-0.1) {Serial.println("close");curDirection = 0; gState = 0; cntms = 0;} break;
          }
        }
        break;
    }*/
  //smoothed printing
  Serial.print(average[0]); 
  Serial.print("\t"); Serial.print(average[1]);
  Serial.print("\t"); Serial.print(average[2]);
  Serial.println();
  }
  if ((currentMillis - previous3000Millis) >= 3000) 
  {
    previous3000Millis = currentMillis;
    //Serial.println("Toggling...");
  }
  /*
  //lsm.read();
  //sensors_event_t a, m, g, temp;
  //lsm.getEvent(&a, &m, &g, &temp); 
  //tmr.update();
  //acceleration m/s^2
  axm = a.acceleration.x;//map(a.acceleration.x,-16*9.8,16*9.8,-32768,32767);
  aym = a.acceleration.y;//map(a.acceleration.y,-16*9.8,16*9.8,-32768,32767);
  azm = a.acceleration.z;//map(a.acceleration.z,-16*9.8,16*9.8,-32768,32767);
  //smooth(Ax, a.acceleration.x);
  //smooth(Ay, a.acceleration.y);
  //smooth(Az, a.acceleration.z);  
  smooth(0, axm);
  smooth(1, aym);
  smooth(2, azm);

  //cntms++;
  
  switch(gState)
  {
    case 0:
      if(average[0]<-0.8)
      {
        curDirection = 1;//left
        //Serial.println("One");
      }
      if(average[0]>0.7)
      {
        curDirection = 2;//right
        //Serial.println("Two");
      }
      if(average[2]<-0.2)
      {
        curDirection = 3;//away
        //Serial.println("Three");
      }
      if(average[2]>1.3)
      {
        curDirection = 4;//closer
        //Serial.println("Four");
      }
      if (curDirection>0) gState = 1;
      break;
    case 1:
      if(cntms==40)
      {
        curDirection = 0;
        gState = 0;
        cntms = 0;
        Serial.println("none");
      }
      else
      {
        switch(curDirection)
        {
          case 1: if(average[0]>0.5) {Serial.println("left");curDirection = 0; gState = 0; cntms = 0;} break;
          case 2: if(average[0]<-0.6)  {Serial.println("right");curDirection = 0; gState = 0; cntms = 0;} break;
          case 3: if(average[2]>1.3)  {Serial.println("away");curDirection = 0; gState = 0; cntms = 0;} break;
          case 4: if(average[2]<-0.1) {Serial.println("close");curDirection = 0; gState = 0; cntms = 0;} break;
        }
      }
      break;
  }
  //smoothed
  //Serial.print(average[0]); 
  //Serial.print("\t"); Serial.print(average[1]);
  //Serial.print("\t"); Serial.print(average[2]);
  //Serial.println();

  //delay(25);
  */
}
void readLSM() 
{
  lsm.read();  /* ask it to read in the data */ 
  lsm.getEvent(&a, &m, &g, &temp);
  cntms++;
}

void smooth(int axis, float val) {
    // pop and subtract the last reading:
    total[axis] -= readings[axis][readIndex[axis]];
    total[axis] += val;

    // add value to running total
    readings[axis][readIndex[axis]] = val;
    readIndex[axis]++;

    if(readIndex[axis] >= numReadings)
        readIndex[axis] = 0;

    // calculate the average:
    average[axis] = total[axis] / numReadings;
}
