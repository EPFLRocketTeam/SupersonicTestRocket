/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.hpp"

// Use array to allow interruption of any sensor
volatile bool flagArray[NUM_SENSORS] = {false};

// Use lambda function for arbitrary interruptions
#define INTERRUPT(i) []() { flagArray[i] = true; }

void acquireData(Sensor *sArray[], size_t sSize, bool serialOutput, XB8XWrapper *xbee)
{
  // SETUP phase
  Serial.print("[acquireData] Entered acquireData\n");

  // Initialize the SD.
  SdFs sd;
  if (!sd.begin(SD_CONFIG))
  {
    sd.initErrorHalt(&Serial);
  }

  if (SERIAL_PRINT)
  {
    Serial.print("[acquireData] SD Card has been set up\n");
  }

  // Initialize the logging file and ring buffer
  FsFile loggingFile;
  RingBuf<FsFile, RING_BUF_CAPACITY> rb;
  if (setupLoggingFile(loggingFile, rb))
  {
    Serial.print("[acquireData] Failure when setting up logging file. Stopping\n");
    return;
  }
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

  if (SERIAL_PRINT)
  {
    Serial.print("[acquireData] Buttons has been set up\n");
  }

  // attach the interrupts
  attachInterrupt(digitalPinToInterrupt(DR_ADIS16470_PIN),
                  INTERRUPT(ADIS16470_INDEX), RISING);
  attachInterrupt(digitalPinToInterrupt(DR_RSC[0]),
                  INTERRUPT(Honeywell_Rsc_0_INDEX), FALLING);
  // attachInterrupt(digitalPinToInterrupt(DR_RSC[1]),
  //                 INTERRUPT(Honeywell_Rsc_1_INDEX), FALLING);
  attachInterrupt(digitalPinToInterrupt(ALTIMAX_DR_PINS[0]),
                  INTERRUPT(Altimax_INDEX), RISING);

  if (SERIAL_PRINT)
  {
    Serial.print("[acquireData] Attached interrupts\n");
  }

  // set the last time for every check to now
  // uint32_t prevSyncLoop = micros();   // timing for syncing
  uint32_t prevSerialLoop = micros(); // timing for serial monitor output
  uint32_t prevRadioLoop = micros();  // timing for radio transmission

  // checking if the sensors are due will update their check times even if we
  // don't use the boolean they return
  // doing regardless of active status since objects are still created
  // if they are inactive, this value will simply never be used

  for (size_t i = 0; i < sSize; i++)
  {
    sArray[i]->isDue(micros(), flagArray[i]);
    if (SERIAL_PRINT)
    {
      Serial.printf("[acquireData] Init sensor number %2d : %s \n", i, sArray[i]->myName());
    }
  }

  // Start acquiring data
  if (SERIAL_PRINT)
  {
    Serial.print("[acquireData] Starting to acquire data.\n");
  }
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // int errorCount = 0;
  //  acquire data as long as button sequence is not initated
  Packet *pkt;
  char line_serial_buffer[LINE_SIZE];
  size_t line_nbr;
  bool printSerial = false;
  bool VRAI = true;

  if (SERIAL_PRINT)
  {
    Serial.print("[acquireData] Reached acquireData loop\n");
  }

  uint32_t start_time = millis();

  while (millis() - start_time < EXPERIMENT_DURATION && checkButtons(buttonArray, stopEvent))
  {
    flashLED(RED_LED_PIN, 100);
    delay(300);
    printSerial = SERIAL_PRINT & (micros() - prevSerialLoop > SERIAL_INTERVAL);

    if (printSerial)
    {
      Serial.write(RESET_TERMINAL);
      Serial.write(HEADER_ERROR_DESC);
      Serial.printf(" (%ld ms)\n", millis());
      Serial.write(HEADER_LINE);
      Serial.write(SEPARATOR_LINE);
    }

    for (size_t i = 0; i < sSize; i++)
    {
      if (sArray[i]->active)
      {

        // Serial.printf("[dataAcquisition] going for sensor %d : %s\n", i, sArray[i]->myName());
        if (sArray[i]->isDue(micros(), flagArray[i]))
        {
          // Serial.printf("[dataAcquisition] getting packet for sensor %d : %s....", i, sArray[i]->myName());
          pkt = sArray[i]->getPacket();
          // Serial.print(" Got it!");
          rb.write(pkt->accessHeader(), sizeof(PacketHeader));
          rb.write(pkt->accessContent(), pkt->getPacketSize());
          // Serial.print(" And wrote it down!!\n");
        }

        if (printSerial)
        {

          pkt = sArray[i]->getPacket();
          memset((void *)line_serial_buffer, '\0', LINE_SIZE);
          pkt->getPrintableHeader(line_serial_buffer);
          line_nbr = 0;
          while (pkt->getPrintableContent(&line_serial_buffer[HEADER_SIZE + 1], line_nbr)) // SIZE does not include \0
          {
            Serial.write(line_serial_buffer, LINE_SIZE);
            memset((void *)line_serial_buffer, '\0', LINE_SIZE);
            snprintf(line_serial_buffer, HEADER_SIZE, HEADER_FILLER_LINE);
            line_nbr++;
          }
          Serial.write(line_serial_buffer, LINE_SIZE);
          Serial.write(SEPARATOR_LINE);
        }
      }
    }
    if (micros() - prevRadioLoop > RADIO_INTERVAL)
    {
      prevRadioLoop = micros();
      Serial.print("Radio sending sensors: ");
      for (size_t i = 0; i < sSize; i++)
      {
        if (sArray[i]->active)
        {
          pkt = sArray[i]->getPacket();
          xbee->send(pkt);
          Serial.printf("%d ",i);
        }
      }
      Serial.print('\n');
    }
      if (printSerial)
      {
        printSerial = false;
        prevSerialLoop = micros();
      }

      // check if it's time to sync
      /*
      if (micros() - prevSyncLoop > SYNC_INTERVAL)
      {
        prevSyncLoop += SYNC_INTERVAL;
        // Serial.println("Syncing data.");
        // Serial.println(micros());
        // TODO: See if better to have with or without syncing.
        // Without syncing seems to write to file anyway since it's preallocated
        // And perhaps solve the issue of unplugging during a sync? To investigate
        // See useful memory time when power loss with and without sync
        // rb.sync();
      }
      */

      // Check if ringBuf is ready for writing
      // Amount of data in ringBuf.
      size_t n = rb.bytesUsed();
      // See if the file is full
      if ((n + loggingFile.curPosition()) > (LOG_FILE_SIZE - 100))
      {

        if (SERIAL_PRINT)
        {
          Serial.print("[acquireData] File full - quiting.\n");
        }
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
          if (SERIAL_PRINT)
          {
            Serial.print("[acquireData] writeOut failed.\n");
          }
          break;
        }
      }

      if (rb.getWriteError())
      {
        // Error caused by too few free bytes in RingBuf.
        if (SERIAL_PRINT)
        {
          Serial.print("[acquireData] WriteError - RingBuf full.\n");
        }
        break;
      }
    } // Finished acquiring data

    if (SERIAL_PRINT)
    {
      Serial.print("[acquireData] Out of acquisition loop\n");
      Serial.printf("[acquireData] Button condition: %d\n", checkButtons(buttonArray, stopEvent));
      Serial.printf("[acquireData] Time condition: %d\n", millis() - start_time < EXPERIMENT_DURATION);
    }

    // detach the interrupts
    detachInterrupt(digitalPinToInterrupt(DR_ADIS16470_PIN));
    detachInterrupt(digitalPinToInterrupt(DR_RSC[0]));
    detachInterrupt(digitalPinToInterrupt(DR_RSC[1]));
    detachInterrupt(digitalPinToInterrupt(ALTIMAX_DR_PINS[0]));

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

    if (SERIAL_PRINT)
    {
      Serial.print("[acquireData] Finished acquiring data\n");
    }
  }

  bool checkButtons(PushButtonArray & buttonArray, uint8_t stopEvent[3])
  // SerialMonitor monitor)
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
        if (SERIAL_PRINT)
        {
          Serial.print("First check to stop acquisition passed.\n");
        }
        Serial.println("First check to stop acquisition passed.");
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
        buttonArray.deactivateEvent(stopEvent[0]);
        buttonArray.activateEvent(stopEvent[1]);
        break;
      case BAD_TRANSITION:
        if (SERIAL_PRINT)
        {
          Serial.print("Button input wrong. Left window.\n");
        }
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        break;
      case WINDOW_START:
        if (SERIAL_PRINT)
        {
          Serial.print("Within window for first check.\n");
        }
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        break;
      case WINDOW_END:
        if (SERIAL_PRINT)
        {
          Serial.print("Left window for first check.\n");
        }
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
        // monitor.writeMessage("Second check to stop "
        //                      "acquisition passed.",
        //                      micros());
        digitalWrite(RED_LED_PIN, LOW);
        buttonArray.deactivateEvent(stopEvent[1]);
        buttonArray.activateEvent(stopEvent[2]);
        break;
      case BAD_TRANSITION:
        if (SERIAL_PRINT)
        {
          Serial.print("Button input wrong. Left window.\n");
        }
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        buttonArray.deactivateEvent(stopEvent[1]);
        buttonArray.activateEvent(stopEvent[0]);
        break;
      case WINDOW_START:
        if (SERIAL_PRINT)
        {
          Serial.print("Within window for second check.\n");
        }
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        break;
      case WINDOW_END:
        if (SERIAL_PRINT)
        {
          Serial.print("Left window for second check.\n");
        }
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
        if (SERIAL_PRINT)
        {
          Serial.print("Passed all checks to stop acquisition.\n");
        }
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        buttonArray.deactivateEvent(stopEvent[2]);
        buttonArray.activateEvent(stopEvent[0]);
        return false;
        break;
      case BAD_TRANSITION:
        if (SERIAL_PRINT)
        {
          Serial.print("Button input wrong. Left window.\n");
        }
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        buttonArray.deactivateEvent(stopEvent[2]);
        buttonArray.activateEvent(stopEvent[0]);
        break;
      case WINDOW_START:
        if (SERIAL_PRINT)
        {
          Serial.print("Within window for third check.\n");
        }
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        break;
      case WINDOW_END:
        if (SERIAL_PRINT)
        {
          Serial.print("Left window for third check.\n");
        }
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        buttonArray.deactivateEvent(stopEvent[2]);
        buttonArray.activateEvent(stopEvent[0]);
        break;
      }
    }

    return true;
  }