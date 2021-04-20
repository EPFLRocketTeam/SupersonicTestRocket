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

// Pins ------------------------------------------------------------------------
// I/O
const byte LED1_PIN = 7,
           LED2_PIN = 8,
           BUTTON0_PIN = 2,
           BUTTON1_PIN = 6;
// ADIS164760
const byte DR_ADIS16470_PIN = 24,
           SYNC_ADIS16470_PIN = 25,
           RST_ADIS16470_PIN = 35,
           CS_ADIS16470_PIN = 36;
// AIS1120SX
const byte CS_AIS1120SX_PIN = 31;
// Pressure sensors
const byte CS_RSC015_EE_PIN = 17,
           CS_RSC015_ADC_PIN = 16,
           DR_RSC015 = 15;
const byte CS_RSC060_EE_PIN = 28,
           CS_RSC060_ADC_PIN = 27,
           DR_RSC060 = 26;
// Thermocouples
const byte CS_TC1 = 23,
           CS_TC2 = 22,
           CS_TC3 = 21,
           CS_TC4 = 20;


// Atmosphere ------------------------------------------------------------------
const float PSI2PA = 6894.76,                                 // [Pa/psi]
            DENSITY = 1.225,                                  // [kg/m^3]
            GAMMA = 1.4,                                      // [-]
            A = 343;                                          // [m/s]

// Timing ----------------------------------------------------------------------
// I/O events. Defined as pairs of button states and durations
const int LOG_ON_STATE = 1;  // State to turn on logging
const unsigned long LOG_ON_DURATION = 1 * 1000 * 1000;        // [us]
const int LOG_OFF_SEQ1_STATE = 1; // State to initiate log off sequence
const unsigned long LOG_OFF_SEQ1_DURATION = 3 * 1000 * 1000;  // [us]
const int LOG_OFF_SEQ2_STATE = 0; // State to continue log off sequence
const unsigned long LOG_OFF_SEQ2_DURATION = 5 * 1000 * 1000;  // [us]
const int LOG_OFF_SEQ3_STATE = 2; // State to finish log off sequence
const unsigned long LOG_OFF_SEQ3_DURATION = 7 * 1000 * 1000;  // [us]
const unsigned long IO_MARGIN = 1 * 1000 * 1000;              // [us]

const int AIS1120SX_LOG_INTERVAL = 2500;                      // [us]
const int TC_LOG_INTERVAL = 100 * 1000;                       // [us]
const int FLUSH_INTERVAL = 500 * 1000;                        // [us]

// I/O -------------------------------------------------------------------------
bool indiv_button_state[2];               // state of the individual buttons
int button_state;                         // current state of button array
// state 0:: button 0: LOW,  button 1: LOW
// state 1:: button 0: HIGH, button 1: LOW
// state 2:: button 0: LOW,  button 1: HIGH
// state 3:: button 0: HIGH, button 1: HIGH
int button_lastState;                     // previous state of button array
unsigned long button_startState[4] = {0}; // moment button array entered state
unsigned long button_endState[4] = {0};   // moment button array left state
unsigned long button_stateTime[4] = {0}; // how long button array was in state

// File logging ----------------------------------------------------------------
// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)

// Size to log 32 byte lines at 2000Hz for more than sixty minutes.
#define LOG_FILE_SIZE 32*2000*3600
// Space to hold 32 byte lines at 2000Hz for more than 1000ms.
#define RING_BUF_CAPACITY 32*2000*1

SdFs sd;
FsFile counterFile;

// Counter for the logging file name
#define COUNTER_FILENAME "counter.dat"
uint16_t counterVal = 0;

// ADIS16470 IMU ---------------------------------------------------------------
// Call ADIS16470 Class
ADIS16470 IMU(CS_ADIS16470_PIN, DR_ADIS16470_PIN, RST_ADIS16470_PIN);
uint8_t *IMUburstData; // Temporary Data Array

// AIS1120SX Accelerometer -----------------------------------------------------

// Pressure sensors ------------------------------------------------------------

// Thermocouples ---------------------------------------------------------------

// Data structures -------------------------------------------------------------

struct datastore {
  unsigned long timestamp;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accX;
  float accY;
  float accZ;
  float accZ_onboard;
  float accZ_altimeter;
  float total_pressure;
  float static_pressure;
  float static_pressure_altimeter;
  float T1;
  float T2;
  float T3;
  float T4;
};
struct datastore myData;


// USER FUNCTIONS ==============================================================

void getButtonArrayState() {
  // gets the state of button array according to the individual button states
  if (indiv_button_state[0] == LOW && indiv_button_state[1] == LOW) {
    button_state = 0;
  } else if (indiv_button_state[0] == HIGH && indiv_button_state[1] == LOW) {
    button_state = 1;
  } else if (indiv_button_state[0] == LOW && indiv_button_state[1] == HIGH) {
    button_state = 2;
  } else {
    button_state = 3;
  }
}

void updateButtonState() {
  // function that updates the current state of the button array

  // start the current state timer
  button_startState[button_state] = micros();
  // end the previous state timer
  button_endState[button_lastState] = button_startState[button_state];
  // calculate duration of previous state
  button_stateTime[button_lastState] = button_endState[button_lastState] \
                                       -  button_startState[button_lastState];
}

void updateButtonTimer() {
  // function that updates the current state of the button array timer
  button_stateTime[button_state] = micros() - button_startState[button_state];
}

void logData() {
  // INITIALIZATION
  // Open or create counter file.
  if (!counterFile.open(COUNTER_FILENAME, O_RDWR | O_CREAT)) {
    Serial.println("open counter file failed\n");
    return;
  }

  // if the file is not empty, get the current count
  // else start at zero
  if (counterFile.size() != 0) {
    counterFile.read(&counterVal, 2);
    counterFile.seek(0); // return to beginning so we don't append to file
  }

  // get the name of the logging file
  char fileName[16];
  snprintf(fileName, sizeof(fileName), "%06d.dat", counterVal);

  // write the count for the next logging file
  counterVal++;
  counterFile.write((const uint8_t *)&counterVal, 2);
  counterFile.close();

  // Open or create logging file.
  FsFile loggingFile;
  if (!loggingFile.open(fileName, O_RDWR | O_CREAT | O_TRUNC)) {
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
  RingBuf<FsFile, RING_BUF_CAPACITY> rb;
  rb.begin(&loggingFile);

  // Max RingBuf used bytes. Useful to understand RingBuf overrun.
  size_t maxUsed = 0;

  // Min spare micros in loop.
  unsigned long minSpareMicros = INT32_MAX;

  // Start time.
  unsigned long nextAISLoop = micros();
  unsigned long nextFlushLoop = micros();

  // Save data into the RingBuf.
  //rb.write((const uint8_t *)&myData, sizeof(myData));

  // Initialize variables to use in the loop
  int logFlag = 1;

  // Start logging
  Serial.println("Logging started. Press the button to stop.");
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  // LOGGING LOOP
  // Log data until button pressed or file full.
  while (logFlag) {
    // Polls each of the timers to see if it's time to do something

    // I/O
    // read the state of the buttons
    indiv_button_state[0] = digitalRead(BUTTON0_PIN);
    indiv_button_state[1] = digitalRead(BUTTON1_PIN);
    getButtonArrayState();

    // If the state changed, due to noise or pressing:
    if (button_state != button_lastState) {
      updateButtonState();
      // if was in state for right amount of time, initialize sequence
      if (button_lastState == LOG_OFF_SEQ1_STATE \
          && button_stateTime[LOG_OFF_SEQ1_STATE] >= LOG_OFF_SEQ1_DURATION \
          && button_stateTime[LOG_OFF_SEQ1_STATE] <= LOG_OFF_SEQ1_DURATION \
          + IO_MARGIN && logFlag == 1) {
        Serial.println("First check to stop logging passed.");
        logFlag = 2;
        digitalWrite(LED1_PIN, LOW);
      } else if (button_lastState == LOG_OFF_SEQ2_STATE \
                 && button_stateTime[LOG_OFF_SEQ2_STATE] >= LOG_OFF_SEQ2_DURATION \
                 && button_stateTime[LOG_OFF_SEQ2_STATE] <= LOG_OFF_SEQ2_DURATION \
                 + IO_MARGIN && logFlag == 2) {
        Serial.println("Second check to stop logging passed.");
        logFlag = 3;
        digitalWrite(LED1_PIN, HIGH);
      } else if (button_lastState == LOG_OFF_SEQ3_STATE \
                 && button_stateTime[LOG_OFF_SEQ3_STATE] >= LOG_OFF_SEQ3_DURATION \
                 && button_stateTime[LOG_OFF_SEQ3_STATE] <= LOG_OFF_SEQ3_DURATION \
                 + IO_MARGIN && logFlag == 3) {
        Serial.println("Third check to stop logging passed. Stopping Logging");
        logFlag = 0;
        digitalWrite(LED1_PIN, LOW);
      } else {
        Serial.println("Check failed. Logging will continue as normal.");
        digitalWrite(LED2_PIN, HIGH);
        logFlag = 1;
      }
    } else {
      updateButtonTimer();
      if (button_state == LOG_OFF_SEQ1_STATE \
          && button_stateTime[LOG_OFF_SEQ1_STATE] >= LOG_OFF_SEQ1_DURATION \
          && button_stateTime[LOG_OFF_SEQ1_STATE] <= LOG_OFF_SEQ1_DURATION \
          + IO_MARGIN && logFlag == 1) {
        digitalWrite(LED1_PIN, HIGH);
        Serial.println("Inside window for first check.");
      } else if (button_state == LOG_OFF_SEQ2_STATE \
          && button_stateTime[LOG_OFF_SEQ2_STATE] >= LOG_OFF_SEQ2_DURATION \
          && button_stateTime[LOG_OFF_SEQ2_STATE] <= LOG_OFF_SEQ2_DURATION \
          + IO_MARGIN && logFlag == 2) {
        digitalWrite(LED1_PIN, HIGH);
        Serial.println("Inside window for second check.");
      } else if (button_state == LOG_OFF_SEQ3_STATE \
          && button_stateTime[LOG_OFF_SEQ3_STATE] >= LOG_OFF_SEQ3_DURATION \
          && button_stateTime[LOG_OFF_SEQ3_STATE] <= LOG_OFF_SEQ3_DURATION \
          + IO_MARGIN && logFlag == 3) {
        digitalWrite(LED1_PIN, HIGH);
        Serial.println("Inside window for third check.");
      } else {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
      }
    }
    button_lastState = button_state;
    
    rb.write((const uint8_t *) 0, 4);
    rb.write((const uint8_t *) micros(), 4);

    // Flush the sd card if it's been long enough
    if (micros() - nextFlushLoop > 0) {
      loggingFile.flush();
      nextFlushLoop += FLUSH_INTERVAL; // restart the flush timer
    }

//    // Read from the AIS1120SX if it's been long enough
//    if (micros() - nextAISLoop > 0) {
//      loggingFile.flush();
//      nextFlushLoop += FLUSH_INTERVAL; // restart the flush timer
//    }

    // See if data is ready for the ADIS16470
    //    // Read IMU burst data and point to data array without checksum
    //    IMUburstData = IMU.byteBurst();

    // See if data is ready for the pressure sensors


    // Check if ringBuf is ready for writing
    // Amount of data in ringBuf.
    size_t n = rb.bytesUsed();
    // See if the file is full
    if ((n + loggingFile.curPosition()) > (LOG_FILE_SIZE - 20)) {
      Serial.println("File full - quiting.");
      break;
    }
    // Update maximum  used buffer size. For buffer overflow issues
    if (n > maxUsed) {
      maxUsed = n;
    }
    if (n >= 512 && !loggingFile.isBusy()) {
      // Not busy only allows one sector before possible busy wait.
      // Write one sector from RingBuf to file.
      if (512 != rb.writeOut(512)) {
        Serial.println("writeOut failed");
        break;
      }
    }

  } // logging while loop

  // Write any RingBuf data to file.
  rb.sync();
  loggingFile.truncate();
  loggingFile.rewind();
  loggingFile.close();
  digitalWrite(LED2_PIN, LOW);

}


// SETUP =======================================================================

void setup() {
  // Open serial communications and give some time for the port to open
  // Not waiting on the port in case the device is not connected to USB
  Serial.begin(9600);
  delay(1000);

  // Set up I/O
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON0_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT);
  Serial.println("I/O has been set up");


  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  delay(500);
  digitalWrite(LED2_PIN, LOW);
  delay(5000);

  // Set up the ADIS16470
  //  IMU.configSPI(); // Configure SPI communication
  //  delay(1000); // Give the part time to start up
  //  IMU.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
  //  IMU.regWrite(DEC_RATE, 0x00); // Set digital filter
  //  IMU.regWrite(FILT_CTRL, 0x04); // Set digital filter
  // Serial.println("ADIS16470 has been set up");


  // Set up the AIS1120SX
  // Serial.println("AIS1120SX has been set up");

  // Set up the pressure sensors
  // Serial.println("Pressure sensors have been set up");

  // Set up the thermocouples
  // Serial.println("Thermocouples have been set up");


  // Initialize the SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
  Serial.println("SD Card has been set up");

  logData();

  // TODO: Add interrupt to start/stop logging
}


// LOOP ========================================================================


void loop() {
  delay(50);
  // I/O
  // read the state of the buttons
  indiv_button_state[0] = digitalRead(BUTTON0_PIN);
  indiv_button_state[1] = digitalRead(BUTTON1_PIN);
  getButtonArrayState();

  // If the state changed, due to noise or pressing:
  if (button_state != button_lastState) {
    updateButtonState();
    // if was in state 1 for right amount of time, start logging
    if (button_lastState == LOG_ON_STATE \
        && button_stateTime[LOG_ON_STATE] >= LOG_ON_DURATION \
        && button_stateTime[LOG_ON_STATE] <= LOG_ON_DURATION + IO_MARGIN) {
      digitalWrite(LED1_PIN, LOW);
      Serial.println("Logging will start.");
      logData();
      Serial.println("Logging stopped.");
    }
  } else {
    updateButtonTimer();
    if (button_state == LOG_ON_STATE \
        && button_stateTime[LOG_ON_STATE] >= LOG_ON_DURATION \
        && button_stateTime[LOG_ON_STATE] <= LOG_ON_DURATION + IO_MARGIN) {
      digitalWrite(LED1_PIN, HIGH);
      Serial.println("Inside 1 to 2 sec window");
    } else {
      digitalWrite(LED1_PIN, LOW);
    }
  }
  button_lastState = button_state;

}

//    // Take data measurements if it's been long enough
//    if (micros() >= nextLogLoop) {
//
//
//      // Read data from all of the sensors.
//      // Done next to one another to be as simultaneous as possible.
//
//      // Put the data into the structure
//      myData.timestamp = micros();
//      myData.gyroX = 0;
//      myData.gyroY = 0;
//      myData.gyroZ = 0;
//      myData.accX = 0;
//      myData.accY = 0;
//      myData.accZ = 0;
//      myData.accZ_onboard = 0;
//      myData.accZ_altimeter = 0;
//      myData.total_pressure = 0;
//      myData.static_pressure = 0;
//      myData.static_pressure_altimeter = 0;
//      myData.T1 = 0;
//      myData.T2 = 0;
//      myData.T3 = 0;
//      myData.T4 = 0;
//
//      // Save data into the RingBuf.
//      rb.write((const uint8_t *)&myData, sizeof(myData));
//
//      if (rb.getWriteError()) {
//        // Error caused by too few free bytes in RingBuf.
//        Serial.println("WriteError");
//        break;
//      }
//
//      if (Serial) { // print data if USB is connected
//        Serial.println("IMU: ");
//        Serial.write(IMUburstData, 20); // Push data pointer to serial port
//        Serial.println("Pressure sensor: ");
//      }
//
//      // Time for next point.
//      nextLogLoop += LOG_INTERVAL_MSEC;
//      unsigned long spareMicros = nextLogLoop - micros();
//      if (spareMicros < minSpareMicros) {
//        minSpareMicros = spareMicros;
//      }
//      if (spareMicros <= 0) {
//        Serial.print("Rate too fast ");
//        Serial.println(spareMicros);
//        break;
//      }
//    } // measurement if statement
