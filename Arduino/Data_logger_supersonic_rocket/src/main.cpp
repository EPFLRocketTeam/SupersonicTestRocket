/*
 * main.cpp Main file that will be used to log data on the Teensy.
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 */

// HEADER ======================================================================
// DATA_LOGGER Script v0.1
// Author: Joshua Cayetano-Emond
// Email: newbie856@gmail.com
// Last updated: 2021-04-21
//
// Changelog:
// v0.1 : first version of the script
//
//
//  This line is exactly 80 characters long. No line should be longer than this.

// IMPORT LIBRARIES ============================================================
// Standard libraries
#include <Arduino.h>
#include <SPI.h>

// User-defined headers
#include "global_variables.h"
#include "PushButtonArray/PushButtonArray.h"

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
const byte CS_TC1_PIN = 23,
           CS_TC2_PIN = 22,
           CS_TC3_PIN = 21,
           CS_TC4_PIN = 20;

void logData()
{
  Serial.println("Logging.");
  digitalWrite(LED2_PIN, HIGH);

  // create the button array
  PushButtonArray buttonArray = PushButtonArray();
  // setup the events
  uint8_t logStop1Event = buttonArray.addEvent(1, 0, 3000, 4000);
  uint8_t logStop2Event = buttonArray.addEvent(0, 2, 5000, 6000);
  uint8_t logStop3Event = buttonArray.addEvent(2, 0, 7000, 8000);
  // deactivate the events for now
  buttonArray.deactivateEvent(logStop2Event);
  buttonArray.deactivateEvent(logStop3Event);
  // setup the output from the event check
  eventOutput eventCheck;

  // repeatedly check if button has been pressed for logging
  bool logFlag = true;
  while (logFlag)
  {
    bool indiv_button_state[2];
    indiv_button_state[0] = digitalRead(BUTTON0_PIN);
    indiv_button_state[1] = digitalRead(BUTTON1_PIN);
    eventCheck = buttonArray.checkEvents(indiv_button_state);

    // if any of the events happened, do something
    if (eventCheck.triggeredEvent == logStop1Event)
    {
      switch (eventCheck.triggeredEventType)
      {
      case NONE:
        break;
      case GOOD_TRANSITION:
        Serial.println("First check to stop logging passed.");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        buttonArray.deactivateEvent(logStop1Event);
        buttonArray.activateEvent(logStop2Event);
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
    if (eventCheck.triggeredEvent == logStop2Event)
    {
      switch (eventCheck.triggeredEventType)
      {
      case NONE:
        break;
      case GOOD_TRANSITION:
        Serial.println("Second check to stop logging passed.");
        digitalWrite(LED1_PIN, LOW);
        buttonArray.deactivateEvent(logStop2Event);
        buttonArray.activateEvent(logStop3Event);
        break;
      case BAD_TRANSITION:
        Serial.println("Button input wrong. Left window.");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        buttonArray.deactivateEvent(logStop2Event);
        buttonArray.activateEvent(logStop1Event);
        break;
      case WINDOW_START:
        Serial.println("Within window for second check.");
        digitalWrite(LED1_PIN, HIGH);
        break;
      case WINDOW_END:
        Serial.println("Left window for second check.");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        buttonArray.deactivateEvent(logStop2Event);
        buttonArray.activateEvent(logStop1Event);
        break;
      }
    }
    if (eventCheck.triggeredEvent == logStop3Event)
    {
      switch (eventCheck.triggeredEventType)
      {
      case NONE:
        break;
      case GOOD_TRANSITION:
        Serial.println("Passed all checks to stop logging.");
        logFlag = false;
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, LOW);
        buttonArray.deactivateEvent(logStop3Event);
        break;
      case BAD_TRANSITION:
        Serial.println("Button input wrong. Left window.");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        buttonArray.deactivateEvent(logStop3Event);
        buttonArray.activateEvent(logStop1Event);
        break;
      case WINDOW_START:
        Serial.println("Within window for third check.");
        digitalWrite(LED1_PIN, HIGH);
        break;
      case WINDOW_END:
        Serial.println("Left window for third check.");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        buttonArray.deactivateEvent(logStop3Event);
        buttonArray.activateEvent(logStop1Event);
        break;
      }
    }
    delay(50);
  }
  digitalWrite(LED1_PIN, LOW);
}

// SETUP =======================================================================

void setup()
{
  // Open serial communications and give some time for the port to open
  // Not waiting on the port in case the device is not connected to USB
  Serial.begin(9600);
  delay(2500);

  // Set up I/O
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON0_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT);
  Serial.println("I/O has been set up");

  // set pins low in case and give some visual feedback
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  delay(500);
  digitalWrite(LED2_PIN, LOW);

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

  // // Initialize the SD.
  // if (!sd.begin(SD_CONFIG))
  // {
  //   sd.initErrorHalt(&Serial);
  // }
  // Serial.println("SD Card has been set up");
  digitalWrite(LED2_PIN, HIGH);
  delay(500);
  digitalWrite(LED2_PIN, LOW);
}

// LOOP ========================================================================

void loop()
{
  // create the button array
  PushButtonArray buttonArray = PushButtonArray();
  // setup the event
  uint8_t logStartEvent = buttonArray.addEvent(1, 0, 1000, 2000);
  // setup the output from the event check
  eventOutput eventCheck;

  // repeatedly check if button has been pressed for logging
  while (true)
  {
    bool indiv_button_state[2];
    indiv_button_state[0] = digitalRead(BUTTON0_PIN);
    indiv_button_state[1] = digitalRead(BUTTON1_PIN);
    eventCheck = buttonArray.checkEvents(indiv_button_state);

    // if any of the events happened, do something
    if (eventCheck.triggeredEvent == logStartEvent)
    {
      switch (eventCheck.triggeredEventType)
      {
      case NONE:
        break;
      case GOOD_TRANSITION:
        Serial.println("Will begin logging as button was pressed.");
        digitalWrite(LED1_PIN, LOW);
        logData();
        break;
      case BAD_TRANSITION:
        Serial.println("Button not pressed properly. Not doing anything.");
        digitalWrite(LED1_PIN, LOW);
        break;
      case WINDOW_START:
        Serial.println("Within window to start logging.");
        digitalWrite(LED1_PIN, HIGH);
        break;
      case WINDOW_END:
        Serial.println("Left window to start logging.");
        digitalWrite(LED1_PIN, LOW);
        break;
      }
    }

    delay(50);
  }
}