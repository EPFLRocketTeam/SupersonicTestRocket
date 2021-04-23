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

    // // check if missed a beat
    // if (micros() - prevAis1120sxLoop > 2 * AIS1120SX_INTERVAL)
    // {
    //   Serial.println("WARNING! Skipped a beat for the AIS1120SX.");
    //   Serial.println("Consider lowering frequency.");
    // }
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
    int8_t ais110sxDue = checkEventDue(micros(), prevAis1120sxLoop,
                                       AIS1120SX_INTERVAL);
    if (ais110sxDue)
    {
      uint8_t errorCode = getErrorCode(ais110sxDue == -1);
      AISx120SXPacket packet(errorCode, micros(), (uint16_t)random(), 0);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }

    // check if it's time to read the thermocouples
    if (checkEventDue(micros(), prevTcLoop, TC_INTERVAL))
    {

      //ThermocouplePacket packet(micros(),
      //rb.write((const uint8_t *)&packet, sizeof(packet));
    }

    // check if it's time to sync
    int8_t syncDue = checkEventDue(micros(), prevSyncLoop, SYNC_INTERVAL);
    if (syncDue)
    {
      uint8_t errorCode = getErrorCode(syncDue == -1);
      Serial.println("Syncing data.");
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

    delayMicroseconds(2000); // eventually remove this

  } // Finished acquiring data

  // CLEANUP Phase
  // Write any RingBuf data to file.
  rb.sync();
  loggingFile.truncate();
  loggingFile.rewind();

  Serial.println("Started converting file.");
  binFileToCSV(loggingFile); // TODO: do this after file has been safely closed
  Serial.println("Finished converting file.");

  loggingFile.close();
  sd.end();

  // Visual cue acquisition has finished
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);
  delay(1000);
  digitalWrite(LED1_PIN, LOW);
  delay(1000);
  digitalWrite(LED1_PIN, HIGH);
  Serial.println("Finished acquiring data");
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
      Serial.println("Second check to stop acquisition passed.");
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
      Serial.println("Passed all checks to stop acquisition.");
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
  return true;
}

int8_t checkEventDue(unsigned long currMicros, unsigned long &prevEvent,
                     unsigned long interval)
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
