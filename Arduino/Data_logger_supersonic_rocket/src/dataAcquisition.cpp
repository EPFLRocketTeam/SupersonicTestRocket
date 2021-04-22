/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.h"

void acquireData()
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

  // Time variables
  unsigned long prevSyncLoop = micros();
  unsigned long prevAis1120sxLoop = micros();
  unsigned long prevTcLoop = micros();
  long minSpareMicros = INT32_MAX; // Min spare micros in loop.

  // Data ready variables
  bool Adis16470DataReadyStates[2] = {0, 0};
  bool RSC015DataReadyStates[2] = {0, 0};
  bool RSC060DataReadyStates[2] = {0, 0};

  // Start acquiring data
  Serial.println("Starting to acquire data.");
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);

  // acquire data as long as button sequence is not initated
  while (checkButtons(buttonArray, stopEvent))
  {
    // Check if ADIS16470 is ready
    // TODO: find a better rising edge detection
    Adis16470DataReadyStates[0] = digitalRead(DR_ADIS16470_PIN);
    if (Adis16470DataReadyStates[0] == 1 &&
        Adis16470DataReadyStates[1] == 0) // rising edge
    {
      Serial.println("Acquiring data from the ADIS16470.");
      // do stuff
      // Save data into the RingBuf.
      // rb.write((const uint8_t *)&myData2, sizeof(myData2));
    }
    Adis16470DataReadyStates[1] = Adis16470DataReadyStates[0];

    // Check if RSC is ready
    // TODO: find a better rising edge detection
    RSC015DataReadyStates[0] = digitalRead(DR_RSC015);
    if (RSC015DataReadyStates[0] == 1 &&
        RSC015DataReadyStates[1] == 0) // rising edge
    {
      Serial.println("Acquiring data from the RSCs.");
      // do stuff
    }

    // check if it's time to read the AIS1120SX
    if (micros() - prevAis1120sxLoop > AIS1120SX_INTERVAL)
    {
      // do stuff
      Serial.println("Acquiring data from the AIS1120SX.");
      prevAis1120sxLoop += AIS1120SX_INTERVAL;
    }

    // check if it's time to read the thermocouples
    if (micros() - prevTcLoop > TC_INTERVAL)
    {
      // do stuff
      Serial.println("Acquiring data from the thermocouples.");
      prevTcLoop += TC_INTERVAL;
    }

    // check if it's time to sync
    if (micros() - prevSyncLoop > SYNC_INTERVAL)
    {
      Serial.println("Syncing data.");
      rb.sync();
      prevSyncLoop += SYNC_INTERVAL;
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

    //delay(100); // eventually remove this

    // check the spare time for each loop
    long spareSyncMicros = prevSyncLoop + SYNC_INTERVAL - micros();
    minSpareMicros = min(minSpareMicros, spareSyncMicros);
    long spareAis1120sxMicros = prevAis1120sxLoop + AIS1120SX_INTERVAL -
                                micros();
    minSpareMicros = min(minSpareMicros, spareAis1120sxMicros);
    long spareTcMicros = prevTcLoop + TC_INTERVAL - micros();
    minSpareMicros = min(minSpareMicros, spareTcMicros);

    
    Serial.println(spareSyncMicros);
    Serial.println(spareAis1120sxMicros);
    Serial.println(spareTcMicros);

    if (minSpareMicros < 0)
    {
      Serial.print("Rate too fast.");
      Serial.println(minSpareMicros);
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

  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);
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
      Serial.println("First check to stop logging passed.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[0]);
      buttonArray.activateEvent(stopEvent[1]);
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
      break;
    case WINDOW_START:
      Serial.println("Within window for first check.");
      digitalWrite(LED1_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for first check.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
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
      Serial.println("Second check to stop logging passed.");
      digitalWrite(LED1_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[2]);
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      Serial.println("Within window for second check.");
      digitalWrite(LED1_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for second check.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
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
      Serial.println("Passed all checks to stop logging.");
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      return false;
      break;
    case BAD_TRANSITION:
      Serial.println("Button input wrong. Left window.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      Serial.println("Within window for third check.");
      digitalWrite(LED1_PIN, HIGH);
      break;
    case WINDOW_END:
      Serial.println("Left window for third check.");
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    }
  }
}