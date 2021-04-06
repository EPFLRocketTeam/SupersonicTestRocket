// DATA_LOGGER Main file that will be used to log data on the Teensy.
//
// HEADER ======================================================================
// DATA_LOGGER Script v0.1
// Author: Joshua Cayetano-Emond
// Email: newbie856@gmail.com
// Last updated: 2021-04-06
//
// Changelog:
// v0.1 : first version of the script
//
//
//  This line is exactly 80 characters long. No line should be longer than this.


// IMPORT LIBRARIES ============================================================
// Standard libraries
#include <SPI.h>

// ADIS16470 Arduino Teensy
// https://github.com/juchong/ADIS16470_Arduino_Teensy
#include <ADIS16470.h>

// SdFat
// https://github.com/greiman/SdFat
#include <SdFat.h>


// DEFINE VARIABLES ============================================================

// Atmosphere
const float PSI2PA = 6894.76,                              // [Pa/psi]
            DENSITY = 1.225,                               // [kg/m^3]
            GAMMA = 1.4,                                   // [-]
            A = 343;                                       // [m/s]

// Timing
unsigned long loopstart;
const int PERIOD = 50;                                     // [ms]

// File logging
String fileName;
//File counterFile, myFile;

// ADIS16470 IMU
const byte IMU_DR_PIN = 2,
           IMU_CS_PIN = 10,
           IMU_RST_PIN = 6;
ADIS16470 IMU(IMU_CS_PIN, IMU_DR_PIN, IMU_RST_PIN); // Call ADIS16470 Class
uint8_t *IMUburstData; // Temporary Data Array

// Accelerometer

// Pressure sensors
const double PSENSOR_MINVOLTAGE = 0.25, // [V]
             PSENSOR_MAXVOLTAGE = 4.5, // [V]
             PSENSOR_RANGE = 15; // [psi]

double Psensor_rawVoltage,
       Q;
const byte PSENSOR_PIN = A4;

// Thermocouples
const byte T1_PIN = A0;
const byte T2_PIN = A1;
const byte T3_PIN = A2;


// USER FUNCTIONS ==============================================================

float subsonic_speed_calculation(float total_pressure, float static_pressure) {
  float M_squared = 2 / (GAMMA - 1) \
    * (pow(total_pressure / static_pressure, (GAMMA - 1) / GAMMA) - 1); 
  return A * sqrt(M_squared);
}


// SETUP =======================================================================

void setup() {
  // Open serial communications and give some time for the port to open
  // Not waiting on the port in case the device is not connected to USB
  Serial.begin(115200);
  delay(1000);

  // Setup the SPI interfaces
  IMU.configSPI(); // Configure SPI communication
  delay(1000); // Give the part time to start up
  IMU.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
  IMU.regWrite(DEC_RATE, 0x00); // Set digital filter
  IMU.regWrite(FILT_CTRL, 0x04); // Set digital filter

  // Setup the I2C interface

  // Setup the analog interface
  pinMode(PSENSOR_PIN, INPUT);
  analogReadResolution(12);

  loopstart = millis();
}


// LOOP ========================================================================

void loop() {
  // Take data measurements if it's been long enough
  if (millis() - loopstart >= PERIOD) {
    // Read data from all of the sensors
    // Read IMU burst data and point to data array without checksum
    IMUburstData = IMU.byteBurst();

    if (Serial) { // print data if USB is connected
      Serial.println("IMU: ");
      Serial.write(IMUburstData, 20); // Push data pointer to serial port
      Serial.println("Pressure sensor: ");
    }
    loopstart = millis(); // restart the loop timer
  }
}
