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

// IMPORT LIBRARIES ============================================================
// Standard libraries
#include <Arduino.h>
#include <SPI.h>

// User-defined headers
#include "globalVariables.h"
#include "PushButtonArray.h"
#include "dataAcquisition.h"
#include "io.h"
//    Sensors
#include "ADIS16470Wrapper.h"
#include "AISx120SXWrapper.h"
#include "HoneywellRscWrapper.h"
#include "MAX31855Wrapper.h"

// DEFINE VARIABLES ============================================================

// Pins ------------------------------------------------------------------------
// I/O
const uint8_t GREEN_LED_PIN = 7,
              RED_LED_PIN = 8,
              BUTTON0_PIN = 2,
              BUTTON1_PIN = 6;
// ADIS164760
const uint8_t DR_ADIS16470_PIN = 24,
              SYNC_ADIS16470_PIN = 25,
              RST_ADIS16470_PIN = 35,
              CS_ADIS16470_PIN = 36;
// AIS1120SX
const uint8_t CS_AIS1120SX_PIN = 31;
// Pressure sensors
const uint8_t CS_RS_EE_PIN[2] = {17, 28};
const uint8_t CS_RSC_ADC_PIN[2] = {16, 27};
const uint8_t DR_RSC[2] = {15, 26};
// Thermocouples
const uint8_t CS_TCS_PIN[4] = {23, 22, 21, 20};

// I/O -------------------------------------------------------------------------
// Button event
const int ACQ_STATE = 1;                // State to turn on acquisition
const int ACQ_NEXT_STATE = 0;           // Next state to turn on
const uint32_t ACQ_WINDOW_START = 1000; // [ms]
const uint32_t ACQ_WINDOW_END = 2000;   // [ms]

// Create the sensor wrapper objects -------------------------------------------
ADIS16470Wrapper adis16470(CS_ADIS16470_PIN, DR_ADIS16470_PIN,
                           RST_ADIS16470_PIN);
AISx120SXWrapper ais1120sx(CS_AIS1120SX_PIN);
HoneywellRscWrapper rscs[2] = {HoneywellRscWrapper(DR_RSC[0], CS_RS_EE_PIN[0],
                                                   CS_RSC_ADC_PIN[0]),
                               HoneywellRscWrapper(DR_RSC[1], CS_RS_EE_PIN[1],
                                                   CS_RSC_ADC_PIN[1])};
MAX31855Wrapper tcs[4];

const int sensorAttempts = 3; // How many times to try to turn on the sensors

// USER FUNCTIONS ==============================================================

// SETUP =======================================================================

void setup()
{
  // Open serial communications and give some time for the port to open
  // Not waiting on the port in case the device is not connected to USB
  Serial.begin(9600);
  delay(2500);

  // Set up I/O
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUTTON0_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT);
  digitalWrite(GREEN_LED_PIN, LOW); // turn off LED in case
  digitalWrite(RED_LED_PIN, LOW);   // turn off LED in case
  Serial.println("I/O has been set up");
  successFlash(); // visual feedback setup is happening

  SPI.begin();

  // Setup the IMU
  if (adis16470.setup(3, 1000))
  {
    Serial.println("ADIS16470 has been set up succesfully.");
    successFlash();
  }
  else
  {
    Serial.println("Could not set up ADIS16470.");
    errorFlash();
  }

  // Setup the AIS1120SX
  if (ais1120sx.setup(3, 1000))
  {
    Serial.println("AIS1120SX has been set up succesfully.");
    successFlash();
  }
  else
  {
    Serial.println("Could not set up AIS1120SX.");
    errorFlash();
  }

  // Setup the pressure sensors
  for (size_t i = 0; i < rscs[i].getSensorQty(); i++)
  {
    if (rscs[i].setup(3, 1000))
    {
      Serial.print("Succesfully started RSC");
      Serial.println(i + 1);
      successFlash();
    }
    else
    {
      Serial.print("Unable to start RSC");
      Serial.println(i + 1);
      errorFlash();
    }
  }

  // Setup the thermocouples
  for (size_t i = 0; i < tcs[i].getSensorQty(); i++)
  {
    if (tcs[i].setup(3, 1000, CS_TCS_PIN[i]))
    {
      Serial.print("Succesfully started thermocouple TC");
      Serial.println(i + 1);
      successFlash();
    }
    else
    {
      Serial.print("Unable to start thermocouple TC");
      Serial.println(i + 1);
      errorFlash();
    }
  }

  Serial.println("Setup complete.");
  successFlash();
}

// LOOP ========================================================================

void loop()
{
  // create the button array
  PushButtonArray buttonArray = PushButtonArray();
  // setup the event
  uint8_t logStartEvent = buttonArray.addEvent(
      ACQ_STATE, ACQ_NEXT_STATE, ACQ_WINDOW_START, ACQ_WINDOW_END);
  // setup the output from the event check
  eventOutput eventCheck;

  // repeatedly check if button has been pressed for data acquistion
  while (true)
  {
    bool indivButtonState[2];
    indivButtonState[0] = digitalRead(BUTTON0_PIN);
    indivButtonState[1] = digitalRead(BUTTON1_PIN);
    eventCheck = buttonArray.checkEvents(millis(), indivButtonState);

    // if any of the events happened, do something
    if (eventCheck.triggeredEvent == logStartEvent)
    {
      switch (eventCheck.triggeredEventType)
      {
      case NONE:
        break;
      case GOOD_TRANSITION:
        Serial.println("Will begin data acquisition as button was pressed.");
        digitalWrite(GREEN_LED_PIN, LOW);
        acquireData(adis16470, ais1120sx, rscs, tcs);
        break;
      case BAD_TRANSITION:
        Serial.println("Button not pressed properly. Not doing anything.");
        digitalWrite(GREEN_LED_PIN, LOW);
        break;
      case WINDOW_START:
        Serial.println("Within window to start data acquisition.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        break;
      case WINDOW_END:
        Serial.println("Left window to start data acquisition.");
        digitalWrite(GREEN_LED_PIN, LOW);
        break;
      }
    }

    delay(50);
  }
}