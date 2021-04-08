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
// Also available from the library manager.
#include <SdFat.h>
#include <RingBuf.h>


// DEFINE VARIABLES ============================================================

// Atmosphere ------------------------------------------------------------------
const float PSI2PA = 6894.76,                              // [Pa/psi]
            DENSITY = 1.225,                               // [kg/m^3]
            GAMMA = 1.4,                                   // [-]
            A = 343;                                       // [m/s]

// Timing ----------------------------------------------------------------------
unsigned long loopstart;
const int LOG_INTERVAL_MSEC = 25;                          // [ms]
const int FLUSH_INTERVAL_MSEC = 1000;                      // [ms]

// File logging ----------------------------------------------------------------
// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO) 

// Size to log 60 byte lines at 40Hz for more than sixty minutes.
#define LOG_FILE_SIZE 60*40*3600  // 8,640,000 bytes.
// Space to hold 60 byte lines at 40Hz for more than 1000ms.
#define RING_BUF_CAPACITY 60*40*1

#define COUNTER_FILENAME "counter.txt"
SdFs sd;
FsFile counterFile, loggingFile;

// RingBuf for File type FsFile.
RingBuf<FsFile, RING_BUF_CAPACITY> rb;

// ADIS16470 IMU ---------------------------------------------------------------
const byte IMU_DR_PIN = 2,
           IMU_CS_PIN = 10,
           IMU_RST_PIN = 6;
ADIS16470 IMU(IMU_CS_PIN, IMU_DR_PIN, IMU_RST_PIN); // Call ADIS16470 Class
uint8_t *IMUburstData; // Temporary Data Array

// Accelerometer ---------------------------------------------------------------

// Pressure sensors ------------------------------------------------------------
// ASCX Differential pressure sensor. To remove eventually
const double PSENSOR_MINVOLTAGE = 0.25, // [V]
             PSENSOR_MAXVOLTAGE = 4.5, // [V]
             PSENSOR_RANGE = 15; // [psi]

double Psensor_rawVoltage,
       Q;
const byte PSENSOR_PIN = A4;

// Thermocouples ---------------------------------------------------------------
const byte T1_PIN = A0;
const byte T2_PIN = A1;
const byte T3_PIN = A2;


// USER FUNCTIONS ==============================================================

void initializeFile() {
  // Initialize the SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }

  char counterString[5] = "0000"; // initialize counter
  // Open or create counter file.
  if (!counterFile.open(COUNTER_FILENAME, O_RDWR | O_CREAT)) {
    Serial.println("open counter file failed\n");
    return;
  }
  // if the file is not empty, get the last line
  if (counterFile.size() != 0) {
    // last line of file. 4 chars + cr + lf
    counterFile.seek(counterFile.size() - 6); 
    for (int i = 0; i < 4; ++i) {
      counterString[i] = counterFile.read();
    }
    counterFile.seek(counterFile.size());
  }
  // write the next file number
  int counterInt;
  sscanf(counterString, "%d", &counterInt); // cast counter into an int
  snprintf(counterString, sizeof(counterString), "%04d",counterInt+1);
  counterFile.println(counterString);
  counterFile.close();

  // open the logging file
  char fileName[16];
  snprintf(fileName, sizeof(fileName), "%04d.txt",counterInt+1);
  // Open or create logging file.
  if (!loggingFile.open(COUNTER_FILENAME, O_RDWR | O_CREAT | O_TRUNC)) {
    Serial.println("open logging file failed\n");
    return;
  }
  // File must be pre-allocated to avoid huge
  // delays searching for free clusters.
  if (!loggingFile.preAllocate(LOG_FILE_SIZE)) {
     Serial.println("preAllocate failed\n");
     loggingFile.close();
     return;
  }
  // initialize the RingBuf.
  rb.begin(&loggingFile);

  // write the header
}

float subsonic_speed_calculation(float total_pressure, float static_pressure) {
  float M_squared = 2 / (GAMMA - 1) \
    * (pow(total_pressure / static_pressure, (GAMMA - 1) / GAMMA) - 1); 
  return A * sqrt(M_squared);
}


// SETUP =======================================================================

void setup() {
  // Open serial communications and give some time for the port to open
  // Not waiting on the port in case the device is not connected to USB
  Serial.begin(9600);
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
  // analogReadResolution(12);

  loopstart = millis();
}


// LOOP ========================================================================

void loop() {
  // Take data measurements if it's been long enough
  if (millis() - loopstart >= LOG_INTERVAL_MSEC) {
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
