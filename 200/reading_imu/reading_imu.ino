/*
2  @file IMU-measurements.ino
3  @author Joshua Marshall (joshua.marshall@queensu.ca), Thomas Sears
(omas.sears@queensu.ca),→
4  @brief Arduino program to read the LSM6DS3 IMU on the Arduino UNO WiFi Rev2.
5  @version 1.1
6  @date 2022-11-30
7 
8  @copyright Copyright (c) 2022
9 
*/

// Need this library installed to access the UNO Wifi Rev2 board's IMU
// For more details, look here:

#include <Arduino_LSM6DS3.h>

// Variables to store angular rates from the gyro [degrees/s]
float omega_x, omega_y, omega_z;

// Variables to store accelerations [g's]
float a_x, a_y, a_z;

// Variables to store sample rates from sensor [Hz]
float a_f, g_f;

float w_x_cal = 0;
float w_y_cal = 0; 
float w_z_cal = 0;
long t_now;
long t_last;
long diff;

void setup()
{
// Open the serial port at 115200 bps
  Serial.begin(115200);

// Wait for serial connection before starting
  while (!Serial)
  {
    delay(10);
  }
  Serial.println();

  // Check that the board is initialized
  if (!IMU.begin())
  {
    // Print an error message if the IMU is not ready
    Serial.print("Failed to initialize IMU :(");
    Serial.print("\n");
    while (1)
    {
      delay(10);
    }
  }

  // Read the sample rate of the accelerometer and gyroscope
  a_f = IMU.accelerationSampleRate();
  g_f = IMU.gyroscopeSampleRate();

  // Print these values to the serial window
  Serial.print("Accelerometer sample rate: ");
  Serial.println(a_f);
  Serial.print("Gyroscope sample rate: ");
  Serial.println(g_f);

  //time parameters to track the sample period for cal values
  t_last = 0;
  t_now = 0;
  diff = t_now - t_last
  
  Serial.println("Now calibrating....")

  //sums values for ten seconds

  while(1)
  {
    if(diff < 10000)
    {
      if (IMU.gyroscopeAvailable())
      {
        IMU.readGyroscope(omega_x, omega_y, omega_z);
        w_x_cal += omega_x;
        w_y_cal += omega_y;
        w_z_cal += omega_z;

      }
      t_now = millis();
      diff = t_now - t_last;

    }else break;
  }

  //divde sum by number of samples (freq * sample_period)
  w_x_cal = w_x_cal/1040;
  w_y_cal = w_y_cal/1040;
  w_z_cal = w_z_cal/1040;

  Serial.println(w_x_cal);
  Serial.println(w_y_cal);
  Serial.println(w_z_cal);


//delay so that cal values can be read easily
  delay(5000);

}

void loop()
{

  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(a_x, a_y, a_z);

    /// Print the accelerometer measurements to the Serial Monitor
    Serial.print("Linear Accelerations:  ");
    Serial.print("a_x: ");
    Serial.print(a_x);
    Serial.print("\t");
    Serial.print("a_y: ");
    Serial.print(a_y);
    Serial.print("\t");
    Serial.print("a_z: ");
    Serial.print(a_z);
    Serial.print(" g\t\t");
  }

  // Read from the gyroscope
  if (IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(omega_x, omega_y, omega_z);

    // Print the gyroscope measurements to the Serial Monitor
    Serial.print("Angular Accelerations:  ");
    Serial.print("w_x: ");
    Serial.print(omega_x - w_x_cal);
    Serial.print("\t");
    Serial.print("w_y: ");
    Serial.print(omega_y - w_y_cal);
    Serial.print("\t");
    Serial.print("w_z: ");
    Serial.print(omega_z - w_z_cal);
    Serial.print(" deg/s");
    Serial.print("\n");
  }

}
