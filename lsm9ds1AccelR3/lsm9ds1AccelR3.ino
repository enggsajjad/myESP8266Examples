#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
//#include <Adafruit_Sensor.h>  // not used in this demo but required!

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
/*
#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5*/
// You can also use software SPI
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_SCK, LSM9DS1_MISO, LSM9DS1_MOSI, LSM9DS1_XGCS, LSM9DS1_MCS);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);

const float xMax1=0.16, xMin1=-0.64, xMax2=-0.1, xMin2=-0.50;
const int xWidth1=4,xSupress1=30,xWidth2=5,xSupress2=35; //in multiples of 50ms
const float yMax1=0.2, yMin1=-0.31, yMax2=-0.05, yMin2=-0.25;
const int yWidth1=4,ySupress1=30,yWidth2=5,ySupress2=35; //in multiples of 50ms
const float zMax1=0.55, zMin1=-0.25, zMax2=0.35, zMin2=0.12;
const int zWidth1=4,zSupress1=30,zWidth2=5,zSupress2=35; //in multiples of 50ms

unsigned char gState =0,cntms=0,curDirection;

const int numReadings = 25;
const int numAxis =3;
boolean flat = false;
uint32_t flatStarted = 0;
uint32_t flatDuration = 0;
uint32_t flatLastEnded = 0;

boolean vertical = false;
uint32_t verticalStarted = 0;
uint32_t verticalDuration = 0;
uint32_t verticalLastEnded = 0;


float readings[numAxis][numReadings];  // the reading history
int readIndex[numAxis];              // the index of the current reading
float total[numAxis];                  // the running total
float average[numAxis];                // the average

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
    while (1);
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
}
float Ax,Ay,Az,Ax2,Ay2,Az2,Mx,My,Mz,Gx,Gy,Gz;
float axm, aym, azm;
void loop() 
{
  yield();
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 
  //acceleration m/s^2
  //Ax = 0.95 * Ax + 0.05 * a.acceleration.x;
  //Ay = 0.95 * Ay + 0.05 * a.acceleration.y;
  //Az = 0.95 * Az + 0.05 * (9.8-a.acceleration.z);
  axm = a.acceleration.x;//map(a.acceleration.x,-16*9.8,16*9.8,-32768,32767);
  aym = a.acceleration.y;//map(a.acceleration.y,-16*9.8,16*9.8,-32768,32767);
  azm = a.acceleration.z;//map(a.acceleration.z,-16*9.8,16*9.8,-32768,32767);
  //smooth(Ax, a.acceleration.x);
  //smooth(Ay, a.acceleration.y);
  //smooth(Az, a.acceleration.z);  
  smooth(0, axm);
  smooth(1, aym);
  smooth(2, azm);
  Ax2 = 0.95 * Ax2 + 0.05 * Ax;
  Ay2 = 0.95 * Ay2 + 0.05 * Ay;
  Az2 = 0.95 * Az2 + 0.05 * Az;
  
  Mx = 0.95 * Mx + 0.05 * m.magnetic.x;
  My = 0.95 * My + 0.05 * m.magnetic.y;
  Mz = 0.95 * Mz + 0.05 * m.magnetic.z;
  Gx = 0.95 * Gx + 0.05 * g.gyro.x;
  Gy = 0.95 * Gy + 0.05 * g.gyro.y;
  Gz = 0.95 * Gz + 0.05 * g.gyro.z;
  cntms++;
  /*
  switch(gState)
  {
    case 0:
      if(average[0]>0.5)
        curDirection = 1;//right
      if(average[0]<-0.7)
        curDirection = 2;//left
      if(average[1]<-0.5)
        curDirection = 3;//away
      if(average[1]>0.7)
        curDirection = 4;//closer
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
          case 1: if(average[0]<-0.5) {Serial.println("right");curDirection = 0; gState = 0; cntms = 0;} break;
          case 2: if(average[0]>0.4)  {Serial.println("left");curDirection = 0; gState = 0; cntms = 0;} break;
          case 3: if(average[1]>0.3)  {Serial.println("away");curDirection = 0; gState = 0; cntms = 0;} break;
          case 4: if(average[1]<-0.4) {Serial.println("close");curDirection = 0; gState = 0; cntms = 0;} break;
        }
      }
      break;
  }*/
  //smoothed
  Serial.print(average[0]); 
  Serial.print("\t"); Serial.print(average[1]);
  Serial.print("\t"); Serial.print(average[2]);
  //mapped
  //Serial.print(axm); 
  //Serial.print("\t"); Serial.print(aym);
  //Serial.print("\t"); Serial.print(azm);
  //Serial.print(a.acceleration.x); 
  //Serial.print("\t"); Serial.print(a.acceleration.y);
  //Serial.print("\t"); Serial.print(a.acceleration.z);

  Serial.println();


  //checkFlat();
  //checkVertical();
  //reportStates();

    //if(wasLifted()){
    //    glowing = true;
    //}

/*
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("\tY: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("\tZ: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");
Serial.println();
*/
  //Serial.println();
  delay(25);
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


void checkFlat(){
    if( abs(average[2]) > 15000 && abs(average[1]) < 5100 ){
    //if( abs(average[2]) > 0.92 && abs(average[1]) < 0.32 ){
        if(!flat){
            flatStarted = millis();
        }
        flatLastEnded = millis();
            
        flatDuration = millis() - flatStarted;
        
        flat = true;
    } else 
    {
        flat = false;
    }
}


void checkVertical(){
    boolean AZ_in_range = abs(average[2]) < 8000;
    boolean AX_in_range = abs(average[0]) < 8000;
    boolean AY_in_range = average[1] < -13500;
    //boolean AZ_in_range = abs(average[2]) < 0.48;//8000;
    //boolean AX_in_range = abs(average[0]) < 0.48;//8000;
    //boolean AY_in_range = average[1] < -0.82;//-13500;
    if( AZ_in_range && AX_in_range && AY_in_range )
    {
        if(!vertical)
        {
            verticalStarted = millis();
        }
        verticalLastEnded = millis();
            
        verticalDuration = millis() - verticalStarted;
        
        vertical = true;
    } else 
    {
        vertical = false;
    }
}


void reportStates(){
    Serial.print("flat: ");
    Serial.print(flat);
    Serial.print(" duration: ");
    Serial.print(flatDuration);
    Serial.print(" since last: ");
    Serial.print(millis() - flatLastEnded);

    Serial.print(" | vertical: ");
    Serial.print(vertical);
    Serial.print(" duration: ");
    Serial.print(verticalDuration);
    Serial.print(" since last: ");
    Serial.print(millis() - verticalLastEnded);

    //Serial.print(" Glowing?: ");
    //Serial.println(glowing);
    Serial.println();
}


boolean wasLifted(){
    if(vertical && verticalDuration > 200 && millis() - flatLastEnded < 1000 && 250 < flatDuration && flatDuration < 2500 ){
        // Sword of Omens, Give Me Sight Beyond Sight!
        return true;
    }

    return false;
}
