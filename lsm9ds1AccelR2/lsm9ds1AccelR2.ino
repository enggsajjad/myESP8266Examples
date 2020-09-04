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

unsigned char gState =0,cnt50ms=0;
void setupSensor()
{
  // 1.) Set the accelerometer range
  ///lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
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
}
float Ax,Ay,Az,Ax2,Ay2,Az2,Mx,My,Mz,Gx,Gy,Gz;
void loop() 
{
  yield();
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 
  //acceleration m/s^2
  Ax = 0.95 * Ax + 0.05 * a.acceleration.x;
  Ay = 0.95 * Ay + 0.05 * a.acceleration.y;
  Az = 0.95 * Az + 0.05 * (9.8-a.acceleration.z);
  Ax2 = 0.95 * Ax2 + 0.05 * Ax;
  Ay2 = 0.95 * Ay2 + 0.05 * Ay;
  Az2 = 0.95 * Az2 + 0.05 * Az;
  
  Mx = 0.95 * Mx + 0.05 * m.magnetic.x;
  My = 0.95 * My + 0.05 * m.magnetic.y;
  Mz = 0.95 * Mz + 0.05 * m.magnetic.z;
  Gx = 0.95 * Gx + 0.05 * g.gyro.x;
  Gy = 0.95 * Gy + 0.05 * g.gyro.y;
  Gz = 0.95 * Gz + 0.05 * g.gyro.z;

  switch (gState)
  {
    case 0:
      if(Ax>xMax1)
      {
        gState =1;
        cnt50ms++;
        Serial.println(Ax);
      }else if(Ax<xMin1)
      {
        gState =3;
        cnt50ms++;
        Serial.println(Ax);
      }else if (Ax >xMax2)
      {
        gState = 6;
        cnt50ms++;
        Serial.println(Ax);
      }else if (Ax < xMin2)
      {
        gState = 9;
        Serial.println(Ax);
        cnt50ms++;
      }
      break;    
    case 1:
      if(Ax>xMax1)
        cnt50ms++;
      if (cnt50ms==xWidth1)
      {
        Serial.println("RIGHT");
        gState=2;
        cnt50ms=0;
      }
      break;
    case 2:
      cnt50ms++;
      if (cnt50ms==xSupress1)
      {
        gState=0;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 3:
      if(Ax<xMin1)
        cnt50ms++;
      if (cnt50ms==xWidth1)
      {
        Serial.println("LEFT");
        gState=2;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 6:
      if(Ax>xMax2)
        cnt50ms++;
      if (cnt50ms==xWidth2)
      {
        Serial.println("RIGHT SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;
    case 4:
      cnt50ms++;
      if (cnt50ms==xSupress2)
      {
        gState=0;
        cnt50ms=0;
      }
      break;  
    ///////////////////////
    case 9:
      if(Ax<xMin2)
        cnt50ms++;
      if (cnt50ms==xWidth2)
      {
        Serial.println("LEFT SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;  
  }
  /*switch (gState)
  {
    case 0:
      if(Ay>yMax1)
      {
        gState =1;
        cnt50ms++;
        Serial.println(Ay);
      }else if(Ay<yMin1)
      {
        gState =3;
        cnt50ms++;
        Serial.println(Ay);
      }else if (Ay >yMax2)
      {
        gState = 6;
        cnt50ms++;
        Serial.println(Ay);
      }else if (Ay < yMin2)
      {
        gState = 9;
        Serial.println(Ay);
        cnt50ms++;
      }
      break;    
    case 1:
      if(Ay>yMax1)
        cnt50ms++;
      if (cnt50ms==yWidth1)
      {
        Serial.println("FORTH");
        gState=2;
        cnt50ms=0;
      }
      break;
    case 2:
      cnt50ms++;
      if (cnt50ms==ySupress1)
      {
        gState=0;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 3:
      if(Ay<yMin1)
        cnt50ms++;
      if (cnt50ms==yWidth1)
      {
        Serial.println("BACK");
        gState=2;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 6:
      if(Ay>yMax2)
        cnt50ms++;
      if (cnt50ms==yWidth2)
      {
        Serial.println("FORTH SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;
    case 4:
      cnt50ms++;
      if (cnt50ms==ySupress2)
      {
        gState=0;
        cnt50ms=0;
      }
      break;  
    ///////////////////////
    case 9:
      if(Ay<yMin2)
        cnt50ms++;
      if (cnt50ms==yWidth2)
      {
        Serial.println("BACK SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;  
  }*/

  /*switch (gState)
  {
    case 0:
      if(Az>zMax1)
      {
        gState =1;
        cnt50ms++;
        Serial.println(Az);
      }else if(Az<zMin1)
      {
        gState =3;
        cnt50ms++;
        Serial.println(Az);
      }else if (Az >zMax2)
      {
        gState = 6;
        cnt50ms++;
        Serial.println(Az);
      }else if (Az < zMin2)
      {
        gState = 9;
        Serial.println(Az);
        cnt50ms++;
      }
      break;    
    case 1:
      if(Az>zMax1)
        cnt50ms++;
      if (cnt50ms==zWidth1)
      {
        Serial.println("DOWN");
        gState=2;
        cnt50ms=0;
      }
      break;
    case 2:
      cnt50ms++;
      if (cnt50ms==zSupress1)
      {
        gState=0;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 3:
      if(Az<zMin1)
        cnt50ms++;
      if (cnt50ms==zWidth1)
      {
        Serial.println("UP");
        gState=2;
        cnt50ms=0;
      }
      break;
    /////////////////////
    case 6:
      if(Az>zMax2)
        cnt50ms++;
      if (cnt50ms==zWidth2)
      {
        Serial.println("DOWN SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;
    case 4:
      cnt50ms++;
      if (cnt50ms==zSupress2)
      {
        gState=0;
        cnt50ms=0;
      }
      break;  
    ///////////////////////
    case 9:
      if(Az<zMin2)
        cnt50ms++;
      if (cnt50ms==zWidth2)
      {
        Serial.println("UP SLOW");
        gState=4;
        cnt50ms=0;
      }
      break;  
  }*/
  /*
  //Dumping data into Excel
  Serial.print(a.acceleration.x); 
  Serial.print("\t"); Serial.print(a.acceleration.y);
  Serial.print("\t"); Serial.print(9.8-a.acceleration.z);
  Serial.print("\t");
  Serial.print(Ax); 
  Serial.print("\t"); Serial.print(Ay);
  Serial.print("\t"); Serial.print(Az);
  //magnetic gauss
  Serial.print("\t");Serial.print(Mx);
  Serial.print("\t"); Serial.print(My);
  Serial.print("\t"); Serial.print(Mz);
  //gyro dps
  Serial.print("\t");Serial.print(Gx);
  Serial.print("\t"); Serial.print(Gy);
  Serial.print("\t"); Serial.print(Gz);
  Serial.println();
  */
  
/*  Serial.print(a.acceleration.x); 
  Serial.print("\t"); Serial.print(a.acceleration.y);
  Serial.print("\t"); Serial.print(a.acceleration.z);
  //magnetic gauss
  Serial.print("\t");Serial.print(m.magnetic.x);
  Serial.print("\t"); Serial.print(m.magnetic.y);
  Serial.print("\t"); Serial.print(m.magnetic.z);
  //gyro dps
  Serial.print("\t");Serial.print(g.gyro.x);
  Serial.print("\t"); Serial.print(g.gyro.y);
  Serial.print("\t"); Serial.print(g.gyro.z);
 Serial.println();
 */
/*
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("\tY: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("\tZ: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");

  Serial.print("Mag X: "); Serial.print(m.magnetic.x);   Serial.print(" gauss");
  Serial.print("\tY: "); Serial.print(m.magnetic.y);     Serial.print(" gauss");
  Serial.print("\tZ: "); Serial.print(m.magnetic.z);     Serial.println(" gauss");

  Serial.print("Gyro X: "); Serial.print(g.gyro.x);   Serial.print(" dps");
  Serial.print("\tY: "); Serial.print(g.gyro.y);      Serial.print(" dps");
  Serial.print("\tZ: "); Serial.print(g.gyro.z);      Serial.println(" dps");
Serial.println();
*/
  //Serial.println();
  delay(50);
}
