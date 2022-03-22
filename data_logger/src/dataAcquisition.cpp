/*
 * dataAcquisition.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "dataAcquisition.hpp"

// Use array to allow interruption of any sensor
volatile bool flagArray[NUM_SENSORS];

// Use lambda function for arbitrary interruptions
#define INTERRUPT(i) []() { flagArray[i] = true; }

void acquireData(Sensor *sArray[], size_t sSize, bool serialOutput, XB8XWrapper* xbee)
{
  // SETUP phase

  // Initialize the SD.
  SdFs sd;
  if (!sd.begin(SD_CONFIG))
  {
    sd.initErrorHalt(&Serial);
  }
  monitor.writeMessage("SD Card has been set up", micros(), true);

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
                  INTERRUPT(ADIS16470_INDEX), RISING);
  attachInterrupt(digitalPinToInterrupt(DR_RSC[0]),
                  INTERRUPT(Honeywell_Rsc_0_INDEX), FALLING);
  attachInterrupt(digitalPinToInterrupt(DR_RSC[1]),
                  INTERRUPT(Honeywell_Rsc_1_INDEX), FALLING);
  attachInterrupt(digitalPinToInterrupt(ALTIMAX_DR_PINS[0]),
                  INTERRUPT(Altimax_INDEX), RISING);

  // set the last time for every check to now
  uint32_t prevSyncLoop = micros();   // timing for syncing
  uint32_t prevSerialLoop = micros(); // timing for serial monitor output
  uint32_t prevRadioLoop = micros(); // timing for radio transmission

  // checking if the sensors are due will update their check times even if we
  // don't use the boolean they return
  // doing regardless of active status since objects are still created
  // if they are inactive, this value will simply never be used

  for (size_t i = 0; i < sSize; i++)
  {
    sArray[i]->isDue(micros(), flagArray[i]);
  }

  // Start acquiring data
  monitor.writeMessage("Starting to acquire data.", micros(), true);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // int errorCount = 0;
  //  acquire data as long as button sequence is not initated
  Packet *pkt;
  char line_serial_buffer[LINE_SIZE];
  size_t line_nbr;
  bool printSerial = false;
  while (checkButtons(buttonArray, stopEvent))
  {
    printSerial = (micros() - prevSerialLoop > SERIAL_INTERVAL);

    if (printSerial)
    {
      Serial.write(RESET_TERMINAL);
      Serial.write(HEADER_ERROR_DESC);
      Serial.write(HEADER_LINE);
      Serial.write(SEPARATOR_LINE);
    }

    for (size_t i = 0; i < sSize; i++)
    {
      if (sArray[i]->active && sArray[i]->isDue(micros(), flagArray[i]))
      {
        pkt = sArray[i]->getPacket(micros());
        rb.write(pkt->accessHeader(), sizeof(PacketHeader));
        rb.write(pkt->accessContent(), pkt->getPacketSize());
      }

      if (micros() - prevRadioLoop > RADIO_INTERVAL)
      {
        xbee->send(pkt);
        prevRadioLoop = micros();
      }

      if (printSerial)
      {
        pkt = sArray[i]->getPacket(micros());
        memset((void *)line_serial_buffer, '\0', LINE_SIZE);
        pkt->getPrintableHeader(line_serial_buffer);
        line_nbr = 0;
        while(pkt->getPrintableContent(&line_serial_buffer[HEADER_SIZE+1],line_nbr)) // SIZE does not include \0
        {
          Serial.write(line_serial_buffer,LINE_SIZE);
          memset((void *)line_serial_buffer, '\0', LINE_SIZE);
          snprintf(line_serial_buffer,HEADER_SIZE,HEADER_FILLER_LINE);
          line_nbr++;
        }
        Serial.write(line_serial_buffer,LINE_SIZE);
        Serial.write(SEPARATOR_LINE);
      }
    }

    if (printSerial)
    {
      printSerial = false;
      prevSerialLoop = micros();
    }
    /* NOT IMPLEMENTED VERBATIM
    // ADIS16470
    if (adis16470.active && adis16470.isDue(micros(), adis16470DRflag))
    {
      ADIS16470Packet packet = adis16470.getPacket(micros());

      // THIS PART IS MISSING (and special to this sensor)
      // What is its purpose ? Why this sensor only ?
      if (packet.header.errorCode)
      {
        errorCount++;
        if (errorCount >= 1000)
        {
          digitalWrite(RED_LED_PIN, HIGH);
        }
      }

      rb.write((const uint8_t *)&packet,
               sizeof(ADIS16470Packet));
    }
    */

    // check if it's time to sync
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

    /* DESACTIVATE IT FOR NOW (adapt it to the virtual Packet)
    // check if it's time to output to the console
    // this is not done with every measurement since the human eye can only
    // see smoothly up to around 60 Hz anyways, and this is only for debugging
    if (micros() - prevSerialLoop > SERIAL_INTERVAL)
    {
      prevSerialLoop += SERIAL_INTERVAL;
      // get the packets for the rsc and max
      HoneywellRSCPacket rscPackets[rscs[0].getSensorQty() * 2] = {};
      MAX31855Packet maxPackets[tcs[0].getSensorQty()] = {};
      for (size_t i = 0; i < rscs[0].getSensorQty(); i++)
      {
        rscPackets[2 * i] = rscs[i].getSerialPackets(micros())[0];
        rscPackets[2 * i + 1] = rscs[i].getSerialPackets(micros())[1];
      }
      for (size_t i = 0; i < tcs[0].getSensorQty(); i++)
      {
        maxPackets[i] = tcs[i].getPacket(micros());
      }
      outputSensorData(micros(), adis16470.getPacket(micros()),
                       ais1120sx.getPacket(micros()),
                       rscPackets, rscs[0].getSensorQty(),
                       maxPackets, tcs[0].getSensorQty());
    }
    */

    // Check if ringBuf is ready for writing
    // Amount of data in ringBuf.
    size_t n = rb.bytesUsed();
    // See if the file is full
    if ((n + loggingFile.curPosition()) > (LOG_FILE_SIZE - 100))
    {

      monitor.writeMessage("File full - quiting.", micros());
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
        monitor.writeMessage("writeOut failed.", micros());
        break;
      }
    }

    if (rb.getWriteError())
    {
      // Error caused by too few free bytes in RingBuf.
      monitor.writeMessage("WriteError - RingBuf full.", micros());
      break;
    }
  } // Finished acquiring data

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

  monitor.writeMessage("Finished acquiring data", micros(), true);
}

bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3],
                  SerialMonitor monitor)
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
      monitor.writeMessage("First check to stop acquisition passed.", micros());
      Serial.println("First check to stop acquisition passed.");
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[0]);
      buttonArray.activateEvent(stopEvent[1]);
      break;
    case BAD_TRANSITION:
      monitor.writeMessage("Button input wrong. Left window.", micros());
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      break;
    case WINDOW_START:
      monitor.writeMessage("Within window for first check.", micros());
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      monitor.writeMessage("Left window for first check.", micros());
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
      monitor.writeMessage("Second check to stop "
                           "acquisition passed.",
                           micros());
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[2]);
      break;
    case BAD_TRANSITION:
      monitor.writeMessage("Button input wrong. Left window.", micros());
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[1]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      monitor.writeMessage("Within window for second check.", micros());
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      monitor.writeMessage("Left window for second check.", micros());
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
      monitor.writeMessage("Passed all checks to stop acquisition.", micros());
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      return false;
      break;
    case BAD_TRANSITION:
      monitor.writeMessage("Button input wrong. Left window.", micros());
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    case WINDOW_START:
      monitor.writeMessage("Within window for third check.", micros());
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      break;
    case WINDOW_END:
      monitor.writeMessage("Left window for third check.", micros());
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      buttonArray.deactivateEvent(stopEvent[2]);
      buttonArray.activateEvent(stopEvent[0]);
      break;
    }
  }
  return true;
}