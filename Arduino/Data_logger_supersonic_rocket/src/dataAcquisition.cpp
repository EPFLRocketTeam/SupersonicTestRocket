/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.h"

void acquireData(ADIS16470 adis16470, AISx120SX ais1120sx,
                 Honeywell_RSC rsc015, Honeywell_RSC rsc060,
                 MAX31855_Class tcs[4])
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
  // start the loop on an even second to make debugging easier
  // TODO: make the second rounding actually work. Far from critical
  uint32_t loopstart = ((uint32_t)micros() / 1000000 + 1) * 1000000;
  uint32_t prevADIS16470loop = loopstart;
  uint32_t prevAis1120sxLoop = loopstart;
  uint32_t prevRSCloop = loopstart;
  uint32_t prevTcLoop = loopstart;
  uint32_t prevSyncLoop = loopstart;

  // Data ready variables
  bool Adis16470DataReadyStates[2] = {0, 0};
  bool RSC015DataReadyStates[2] = {0, 0};
  bool RSC060DataReadyStates[2] = {0, 0};

  // Start acquiring data
  Serial.println("Starting to acquire data.");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // acquire data as long as button sequence is not initated
  while (checkButtons(buttonArray, stopEvent))
  {

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
      uint8_t errorCode = getErrorCode(adis16470Due == -1, 1, 0,
                                       wordBurstData[9] == checksum);

      // create and write the packet
      ADIS16470Packet packet(errorCode, micros(), wordBurstData);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }
    // Check if ADIS16470 is ready because of the data ready signal
    // TODO: find a better rising edge detection
    Adis16470DataReadyStates[0] = digitalRead(DR_ADIS16470_PIN);
    // rising edge is when current reading is high and last reading is low
    if (Adis16470DataReadyStates[0] == 1 &&
        Adis16470DataReadyStates[1] == 0)
    {
      prevADIS16470loop += micros(); // reset the timer now that DR works
      Serial.println("Acquiring data from the ADIS16470 due to DR.");
      // acquire the data
      uint16_t *wordBurstData;
      wordBurstData = adis16470.wordBurst(); // Read data and insert into array

      // check for errors
      int16_t checksum = adis16470.checksum(wordBurstData);
      uint8_t errorCode = getErrorCode(adis16470Due == -1, 0, 0,
                                       wordBurstData[9] == checksum);

      // create and write the packet
      ADIS16470Packet packet(errorCode, micros(), wordBurstData);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }
    Adis16470DataReadyStates[1] = Adis16470DataReadyStates[0];

    // check if it's time to read the AIS1120SX
    int8_t ais110sxDue = checkEventDue(micros(), prevAis1120sxLoop,
                                       AIS1120SX_INTERVAL);
    if (ais110sxDue)
    {
      Serial.println("Acquiring data from the AIS1120SX");
      uint8_t errorCode = getErrorCode(ais110sxDue == -1, 0, 0, 0);
      AISx120SXPacket packet(errorCode, micros(), (uint16_t)random(), 0);
      rb.write((const uint8_t *)&packet, sizeof(packet));
    }

    // Check if RSC is ready
    // TODO: find a better rising edge detection
    RSC015DataReadyStates[0] = digitalRead(DR_RSC015);
    if (RSC015DataReadyStates[0] == 1 &&
        RSC015DataReadyStates[1] == 0) // rising edge
    {
      Serial.println("Acquiring data from the RSCs.");
      // do stuff
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