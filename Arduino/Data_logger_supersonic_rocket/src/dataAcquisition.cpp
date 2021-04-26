/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.h"

void acquireData(ADIS16470 adis16470, AISx120SX ais1120sx,
                 Honeywell_RSC rscs[2], MAX31855_Class tcs[4])
{
  // SETUP phase

  // Initialize the SD.
  SdFs sd;
  if (!sd.begin(SD_CONFIG))
  {
    sd.initErrorHalt(&Serial);
  }
  Serial.println("SD Card has been set up");

  // Initialize the logging file and ring buffer
  FsFile loggingFile;
  RingBuf<FsFile, RING_BUF_CAPACITY> rb;
  setupLoggingFile(loggingFile, rb);
  // Max RingBuf used bytes. Useful to understand RingBuf overrun.
  size_t maxUsed = 0;

  // Setup the buttons
  PushButtonArray buttonArray = PushButtonArray(); // create the button array
  // setup the events
  uint8_t stopEvent[3] = {
      buttonArray.addEvent(STOP_STATES[0], NEXT_STOP_STATES[0],
                           STOP_WINDOW_START[0], STOP_WINDOW_END[0]),
      buttonArray.addEvent(STOP_STATES[1], NEXT_STOP_STATES[1],
                           STOP_WINDOW_START[1], STOP_WINDOW_END[1]),
      buttonArray.addEvent(STOP_STATES[2], NEXT_STOP_STATES[0],
                           STOP_WINDOW_START[2], STOP_WINDOW_END[2])};
  // deactivate the second and third events for now
  buttonArray.deactivateEvent(stopEvent[1]);
  buttonArray.deactivateEvent(stopEvent[2]);

  // Quantity of sensors
  size_t rscNum = sizeof(rscs) / sizeof(rscs[0]);
  size_t tcNum = sizeof(tcs) / sizeof(tcs[0]);

  // Data ready variables
  bool Adis16470DataReadyStates[2] = {0};
  bool RscDataReadyStates[rscNum][2] = {0};

  // Last measurements
  int16_t prevProbeMeas[tcNum] = {0};
  int16_t prevAmbientMeas[tcNum] = {0};
  int16_t prevAis1120sxMeas[2] = {0};

  // 

  // Time variables
  // start the loop on an even second to make debugging easier
  // TODO: make the second rounding actually work. Far from critical
  uint32_t loopstart = ((uint32_t)micros() / 1000000 + 1) * 1000000;
  uint32_t prevADIS16470loop = loopstart;
  uint32_t prevAis1120sxLoop = loopstart;
  uint32_t prevAis1120sxMeasTime = loopstart;
  uint32_t prevRSCloop[rscNum] = {loopstart};
  uint32_t prevTcLoop = loopstart;
  uint32_t prevTcMeasTime[tcNum] = {loopstart};
  uint32_t prevSyncLoop = loopstart;

  // Start acquiring data
  Serial.println("Starting to acquire data.");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // acquire data as long as button sequence is not initated
  while (checkButtons(buttonArray, stopEvent))
  {

    // ADIS16470
    // check if the ADIS16470 is ready because of time
    // if it is, it means that the data ready line isn't working properly
    // adding a small margin to allow for timing inconsistencies with DR line
    int8_t adis16470Due = checkEventDue(micros() + ADIS16470_MARGIN,
                                        prevADIS16470loop, ADIS16470_INTERVAL);
    if (adis16470Due)
    {
      Serial.println("Acquiring data from the ADIS16470 due to time.");
      // acquire the data
      uint16_t *wordBurstData;
      wordBurstData = adis16470.wordBurst(); // Read data and insert into array

      // check for errors
      int16_t checksum = adis16470.checksum(wordBurstData);
      uint8_t errorCode = getErrorCode(adis16470Due == -1, 0, 1,
                                       wordBurstData[9] == checksum);

      // create and write the packet
      ADIS16470Packet packet(0, errorCode, micros(), wordBurstData);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }
    // Check if ADIS16470 is ready because of the data ready signal
    // TODO: find a better rising edge detection
    Adis16470DataReadyStates[0] = digitalRead(DR_ADIS16470_PIN);
    // rising edge is when current reading is high and last reading is low
    if (Adis16470DataReadyStates[0] == 1 &&
        Adis16470DataReadyStates[1] == 0)
    {
      prevADIS16470loop = micros(); // reset the timer now that DR works
      Serial.println("Acquiring data from the ADIS16470 due to DR.");
      // acquire the data
      uint16_t *wordBurstData;
      wordBurstData = adis16470.wordBurst(); // Read data and insert into array

      // check for errors
      int16_t checksum = adis16470.checksum(wordBurstData);
      uint8_t errorCode = getErrorCode(adis16470Due == -1, 0, 0,
                                       wordBurstData[9] == checksum);

      // create and write the packet
      ADIS16470Packet packet(0, errorCode, micros(), wordBurstData);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }
    Adis16470DataReadyStates[1] = Adis16470DataReadyStates[0];

    // AIS1120SX
    // check if it's time to read the AIS1120SX
    // checking around 10 times more often than the sampling frequency to
    // get data as soon as it comes out
    int8_t ais110sxDue = checkEventDue(micros(), prevAis1120sxLoop,
                                       AIS1120SX_CHECK_INTERVAL);
    if (ais110sxDue)
    {
      // read the data
      int16_t *rawData;
      rawData = ais1120sx.readData();

      // check if the new measurement is different from the last one
      if (memcmp(rawData, prevAis1120sxMeas, sizeof(*rawData)) != 0)
      {
        Serial.println("Acquiring data from the AIS1120SX");
        // see if the new measurements are late
        bool measSkippedBeat = (micros() - prevAis1120sxMeasTime >
                                2 * AIS1120SX_NOM_INTERVAL);

        // get the error code
        uint8_t errorCode = getErrorCode(measSkippedBeat, ais110sxDue == -1, 0, 0);
        //uint16_t *accelData;

        // create and send packet
        AISx120SXPacket packet(0, errorCode, micros(), rawData);
        rb.write((const uint8_t *)&packet, sizeof(packet));

        // save last measurements and when they were obtained
        memcpy(prevAis1120sxMeas, rawData, sizeof(prevAis1120sxMeas));
        prevAis1120sxMeasTime = micros();
      }
    }

    // RSC
    // loop through all RSCs
    for (size_t i = 0; i < rscNum; i++)
    {
      // check if the RSC is ready because of time
      // if it is, it means that the data ready line isn't working properly
      // adding a small margin to allow for timing inconsistencies with DR line

      int8_t rscDue = checkEventDue(micros() + HONEYWELL_RSC_MARGIN,
                                    prevRSCloop[i], HONEYWELL_RSC_INTERVAL);
      if (rscDue)
      {
        Serial.print("Acquiring data from the RSC");
        Serial.print(i);
        Serial.println(" due to time.");

        // acquire the data
        float meas = 0;

        // check for errors
        uint8_t errorCode = getErrorCode(rscDue == -1, 0, 1, 0);

        // create and write the packet
        HoneywellRSCPressurePacket packet(i, errorCode, micros(), meas);
        rb.write((const uint8_t *)&packet, sizeof(packet));
      }
      // Check if RSC is ready because of the data ready signal
      // TODO: find a better rising edge detection
      RscDataReadyStates[i][0] = digitalRead(DR_RSC[i]);
      // rising edge is when current reading is high and last reading is low
      if (RscDataReadyStates[i][0] == 1 &&
          RscDataReadyStates[i][1] == 0)
      {
        prevRSCloop[i] = micros(); // reset the timer now that DR works
        Serial.print("Acquiring data from the RSC");
        Serial.print(i);
        Serial.println(" due to DR.");

        // acquire the data
        float meas = 0;

        // check for errors
        uint8_t errorCode = getErrorCode(rscDue == -1, 0, 1, 0);

        // create and write the packet
        HoneywellRSCPressurePacket packet(i, errorCode, micros(), meas);
        rb.write((const uint8_t *)&packet, sizeof(packet));
      }
      RscDataReadyStates[i][1] = RscDataReadyStates[i][0];
    }

    // THERMOCOUPLES
    // check if it's time to read the thermocouples
    // checking around 10 times more often than the sampling frequency to
    // get data as soon as it comes out
    int8_t tcDue = checkEventDue(micros(), prevTcLoop, TC_CHECK_INTERVAL);
    if (tcDue)
    {
      // loop through all thermocouples
      for (size_t i = 0; i < tcNum; i++)
      {
        // read the measurements from the sensor
        int32_t rawMeas = tcs[i].readRaw();
        int16_t probeT = tcs[i].rawToProbe(rawMeas);
        int16_t ambientT = tcs[i].rawToAmbient(rawMeas);

        // check if the new measurement is different from the last one
        // and that it does not contain an error
        if (probeT != prevProbeMeas[i] && ambientT != prevAmbientMeas[i] &&
            probeT != INT16_MAX && ambientT != INT16_MAX)
        {
          Serial.print("Acquiring data from TC");
          Serial.println(i);
          // see if the new measurements are late
          bool measSkippedBeat = (micros() - prevTcMeasTime[i] >
                                  2 * TC_NOM_INTERVAL);
          // get the error code
          uint8_t errorCode = getErrorCode(measSkippedBeat, tcDue, 0, 0);

          // get and send the packet
          ThermocouplePacket packet(i, errorCode, micros(), probeT, ambientT);
          rb.write((const uint8_t *)&packet, sizeof(packet));

          // save last measurements and when they were obtained
          prevProbeMeas[i] = probeT;
          prevAmbientMeas[i] = ambientT;
          prevTcMeasTime[i] = micros();
        }
      }
    }

    // check if it's time to sync
    int8_t syncDue = checkEventDue(micros(), prevSyncLoop, SYNC_INTERVAL);
    if (syncDue)
    {
      uint8_t errorCode = getErrorCode(syncDue == -1, 0, 0, 0);
      Serial.println("Syncing data.");
      // Serial.println(micros());
      // TODO: See if better to have with or without syncing.
      // Without syncing seems to write to file anyway since it's preallocated
      // And perhaps solve the issue of unplugging during a sync? To investigate
      // See useful memory time when power loss with and without sync
      // rb.sync();
    }

    // Check if ringBuf is ready for writing
    // Amount of data in ringBuf.
    size_t n = rb.bytesUsed();
    // See if the file is full
    if ((n + loggingFile.curPosition()) > (LOG_FILE_SIZE - 100))
    {
      Serial.println("File full - quiting.");
      break;
    }
    // Update maximum  used buffer size. For buffer overflow issues
    if (n > maxUsed)
    {
      maxUsed = n;
    }
    if (n >= 512 && !loggingFile.isBusy())
    {
      // Not busy only allows one sector before possible busy wait.
      // Write one sector from RingBuf to file.
      if (512 != rb.writeOut(512))
      {
        Serial.println("writeOut failed");
        break;
      }
    }

    if (rb.getWriteError())
    {
      // Error caused by too few free bytes in RingBuf.
      Serial.println("WriteError");
      break;
    }

  } // Finished acquiring data

  // CLEANUP Phase
  // Write any RingBuf data to file.
  rb.sync();
  loggingFile.truncate();
  loggingFile.rewind();
  loggingFile.close();
  sd.end();

  // Visual cue acquisition has finished
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  successFlash();
  Serial.println("Finished acquiring data");
}

int8_t checkEventDue(uint32_t currMicros, uint32_t &prevEvent,
                     uint32_t interval)
{
  // check if missed a beat
  if (currMicros - prevEvent > 2 * interval)
  {
    int beatsSkipped = floor((currMicros - prevEvent) / interval);
    Serial.print("WARNING! Skipped following amount of beats:");
    Serial.println(beatsSkipped - 1);
    Serial.println("Consider lowering frequency.");
    prevEvent += beatsSkipped * interval; // catch up
    return -1;                            // event is due but an error happened
  }
  else if (currMicros - prevEvent > interval) // check if the event is due
  {
    prevEvent += interval;
    return 1; // event is due
  }
  else
  {
    return 0; // event is not due
  }
}

bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3])
{
  // see which buttons were pressed
  bool indivButtonState[2];
  indivButtonState[0] = digitalRead(BUTTON0_PIN);
  indivButtonState[1] = digitalRead(BUTTON1_PIN);
  eventOutput eventCheck = buttonArray.checkEvents(indivButtonState);

  // if any of the events happened, do something
  if (eventCheck.triggeredEvent == stopEvent[0])
  {
    switch (eventCheck.triggeredEventType)
    {
    case NONE:
      break;
    case GOOD_TRANSITION:
      Serial.println("First check to stop acquisition passed.");
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[0]);
      buttonArray.activateEvent(stopEvent[1]);
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      break;
    case WINDOW_START:
      Serial.println("Within window for first check.");
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for first check.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      break;
    }
  }
  if (eventCheck.triggeredEvent == stopEvent[1])
  {
    switch (eventCheck.triggeredEventType)
    {
    case NONE:
      break;
    case GOOD_TRANSITION:
      Serial.println("Second check to stop acquisition passed.");
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[2]);
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      Serial.println("Within window for second check.");
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for second check.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    }
  }
  if (eventCheck.triggeredEvent == stopEvent[2])
  {
    switch (eventCheck.triggeredEventType)
    {
    case NONE:
      break;
    case GOOD_TRANSITION:
      Serial.println("Passed all checks to stop acquisition.");
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      return false;
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      Serial.println("Within window for third check.");
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for third check.");
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    }
  }
  return true;
}