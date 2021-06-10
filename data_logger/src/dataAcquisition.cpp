/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.h"

// flags for the data ready triggers
volatile bool adis16470DRflag = false;
volatile bool rscDRflag[2] = {0};

void interruptFunctionADIS()
{
  adis16470DRflag = true;
}
void interruptFunctionRSC1()
{
  rscDRflag[0] = true;
}
void interruptFunctionRSC2()
{
  rscDRflag[1] = true;
}

void acquireData(ADIS16470Wrapper adis16470, AISx120SXWrapper ais1120sx,
                 HoneywellRscWrapper *rscs, MAX31855Wrapper *tcs)
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

  // attach the interrupts
  attachInterrupt(digitalPinToInterrupt(DR_ADIS16470_PIN),
                  interruptFunctionADIS, RISING);
  attachInterrupt(digitalPinToInterrupt(DR_RSC[0]),
                  interruptFunctionRSC1, FALLING);
  attachInterrupt(digitalPinToInterrupt(DR_RSC[1]),
                  interruptFunctionRSC2, FALLING);

  uint32_t prevSyncLoop = micros(); // timing for syncing

  // Start acquiring data
  Serial.println("Starting to acquire data.");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // acquire data as long as button sequence is not initated
  while (checkButtons(buttonArray, stopEvent))
  {

    // ADIS16470
    if (adis16470.active) // check if the sensor is active
    {
      if (adis16470.isDue(micros(), adis16470DRflag)) // if due
      {
        Serial.println("Acquiring data from the ADIS16470.");
        ADIS16470Packet packet = adis16470.getPacket(micros());
        rb.write((const uint8_t *)&packet, sizeof(packet));
      }
    }

    // AIS1120SX
    if (ais1120sx.active) // check if the sensor is active
    {
      if (ais1120sx.isDue(micros())) // if due
      {
        Serial.println("Acquiring data from the AIS1120SX.");
        AISx120SXPacket packet = ais1120sx.getPacket(micros());
        rb.write((const uint8_t *)&packet, sizeof(packet));
      }
    }

    // Pressure sensors
    for (size_t i = 0; i < rscs[i].getSensorQty(); i++)
    {
      if (rscs[i].active) // check if the sensor is active
      {
        if (rscs[i].isDue(micros(), rscDRflag[i])) // sensor is due
        {
          Serial.print("Acquiring data from RSC");
          Serial.println(i + 1);
          HoneywellRSCPacket packet = rscs[i].getPacket(micros());
          rb.write((const uint8_t *)&packet, sizeof(packet));
        }
      }
    }

    // THERMOCOUPLES
    for (size_t i = 0; i < tcs[i].getSensorQty(); i++)
    {
      if (tcs[i].active) // check if the sensor is active
      {
        if (tcs[i].isDue(micros())) // check if sensor is due
        {
          Serial.print("Acquiring data from TC");
          Serial.println(i + 1);
          MAX31855Packet packet = tcs[i].getPacket(micros());
          rb.write((const uint8_t *)&packet, sizeof(packet));
        }
      }
    }

    // check if it's time to sync
    if (micros() - prevSyncLoop > SYNC_INTERVAL)
    {
      prevSyncLoop += SYNC_INTERVAL;
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

  // detach the interrupts
  detachInterrupt(digitalPinToInterrupt(DR_ADIS16470_PIN));
  detachInterrupt(digitalPinToInterrupt(DR_RSC[0]));
  detachInterrupt(digitalPinToInterrupt(DR_RSC[1]));

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

bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3])
{
  // see which buttons were pressed
  bool indivButtonState[2];
  indivButtonState[0] = digitalRead(BUTTON0_PIN);
  indivButtonState[1] = digitalRead(BUTTON1_PIN);
  eventOutput eventCheck = buttonArray.checkEvents(millis(), indivButtonState);

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
      digitalWrite(GREEN_LED_PIN, LOW);
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
      digitalWrite(GREEN_LED_PIN, LOW);
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
      digitalWrite(GREEN_LED_PIN, LOW);
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