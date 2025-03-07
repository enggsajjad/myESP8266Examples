#include <Wire.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_L3GD20_U.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_LSM9DS1.h>

// Create sensor instances.
//Adafruit_L3GD20_Unified       gyro(20);
//Adafruit_LSM303_Accel_Unified accel(30301);
//Adafruit_LSM303_Mag_Unified   mag(30302);

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

// This sketch can be used to output raw sensor data in a format that
// can be understoof by MotionCal from PJRC. Download the application
// from http://www.pjrc.com/store/prop_shield.html and make note of the
// magentic offsets after rotating the sensors sufficiently.
//
// You should end up with 3 offsets for X/Y/Z, which are displayed
// in the top-right corner of the application.
//
// Make sure you have the latest versions of the Adfruit_L3GD20_U and
// Adafruit_LSM303_U libraries when running this sketch since they
// use a new .raw field added in the latest versions (Oct 10 2016)
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

  // Wait for the Serial Monitor to open (comment out to run without Serial Monitor)
  // while(!Serial);
  
  Serial.println(F("Adafruit 10 DOF Board AHRS Calibration Example")); Serial.println("");
  /*
  // Initialize the sensors.
  if(!gyro.begin())
  {
    // There was a problem detecting the L3GD20 ... check your connections 
    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
    while(1);
  }
  
  if(!accel.begin())
  {
    // There was a problem detecting the LSM303DLHC ... check your connections 
    Serial.println("Ooops, no L3M303DLHC accel detected ... Check your wiring!");
    while(1);
  }
  
  if(!mag.begin())
  {
    // There was a problem detecting the LSM303DLHC ... check your connections 
    Serial.println("Ooops, no L3M303DLHC mag detected ... Check your wiring!");
    while(1);
  }*/

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

void loop(void)
{
  /*sensors_event_t event; // Need to read raw data, which is stored at the same time
  
  // Get new data samples
  gyro.getEvent(&event);
  accel.getEvent(&event);
  mag.getEvent(&event);

  // Print the sensor data
  Serial.print("Raw:");
  Serial.print(accel.raw.x);
  Serial.print(',');
  Serial.print(accel.raw.y);
  Serial.print(',');
  Serial.print(accel.raw.z);
  Serial.print(',');
  Serial.print(gyro.raw.x);
  Serial.print(',');
  Serial.print(gyro.raw.y);
  Serial.print(',');
  Serial.print(gyro.raw.z);
  Serial.print(',');
  Serial.print(mag.raw.x);
  Serial.print(',');
  Serial.print(mag.raw.y);
  Serial.print(',');
  Serial.print(mag.raw.z);
  Serial.println();  
  */
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 
  
      // Print the sensor data
  Serial.print("Raw:");
  Serial.print(a.acceleration.x);
  Serial.print(',');
  Serial.print(a.acceleration.y);
  Serial.print(',');
  Serial.print(a.acceleration.z);
  Serial.print(',');
  Serial.print(g.gyro.x);
  Serial.print(',');
  Serial.print(g.gyro.y);
  Serial.print(',');
  Serial.print(g.gyro.z);
  Serial.print(',');
  Serial.print(m.magnetic.x);
  Serial.print(',');
  Serial.print(m.magnetic.y);
  Serial.print(',');
  Serial.print(m.magnetic.z);
  Serial.println();
  delay(50);
}
