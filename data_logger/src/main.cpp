/**
 * @file main.cpp
 * @author Joshua Cayetano-Emond (joshua.cayetano.emond@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-04-21
 *
 * @copyright Copyright (c) 2021
 *
 */

/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

/*! \page fooPage foo Functions
 *
 * - \subpage Copy
 */

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
#include "globalVariables.hpp"
#include "PushButtonArray.h"
#include "dataAcquisition.hpp"
#include "io.hpp"
//    Sensors
#include "Sensors/ADIS16470Wrapper.hpp"
#include "Sensors/AISx120SXWrapper.hpp"
#include "Sensors/AltimaxWrapper.hpp"
#include "Sensors/HoneywellRscWrapper.hpp"
#include "Sensors/MAX31855Wrapper.hpp"
#include "Sensors/MAX7Wrapper.hpp"
#include "Sensors/AD8556Wrapper.hpp"

// XBee
#include "XB8XWrapper.hpp"

// Debug
#include <ArduinoTrace.h>

// DEFINE VARIABLES ============================================================

// Analog ----------------------------------------------------------------------

const int ANALOG_READ_RESOLUTION = 12;
const int ANALOG_READ_AVERAGING = 4;

// Pins ------------------------------------------------------------------------
// I/O
const uint8_t GREEN_LED_PIN = 5,
              RED_LED_PIN = 3,
              BUTTON0_PIN = 21,
              BUTTON1_PIN = 36;
// ADIS164760
const uint8_t DR_ADIS16470_PIN = 10,
              SYNC_ADIS16470_PIN = 24,
              RST_ADIS16470_PIN = 29,
              CS_ADIS16470_PIN = 28;
// AIS1120SX
const uint8_t CS_AIS1120SX_PIN = 7;
// Pressure sensors
const uint8_t staticRSC = 0;
const uint8_t totalRSC = 1;
const uint8_t CS_RS_EE_PIN[2] = {2, 8};
const uint8_t CS_RSC_ADC_PIN[2] = {22, 36};
const uint8_t DR_RSC[2] = {23, 15};
// Thermocouples
const uint8_t TAT_TC = 3;
const uint8_t CS_TCS_PIN[4] = {1, 0, 21, 35};
// Altimax
const uint8_t ALTIMAX_DR_PINS[3] = {33, 255, 255}; // 255 for not implemented

// XBee
const uint8_t XBEE_PINS[] = {34, 33}; // Rx, Tx pins, connected to Serial5

// MAX-7
const uint8_t MAX7_PINS[] = {31, 32}; // Rx, Tx pins, connected to Serial4

// AD8556
const uint8_t AD8556_VPLUS = 19;
const uint8_t AD8556_VMINUS = 18;

const float AD8556_MIN_READ = 0;

//// maxReading at 3.3V ; at 0.1 V get 200 lbs, converted to kg, then multiply g to get Newtons
//const float AD8556_MAX_READ = 3.3 * (200 / 0.1) * 0.45359237 * 9.80665;

///maxReading at 3.3V -> get 200 lbs, converted to kg, then multiply g to get Newtons
const float AD8556_MAX_READ = 3.3 * 200  * 0.45359237 * 9.80665;

// I/O -------------------------------------------------------------------------
// Button event
const int ACQ_STATE = 1;                // State to turn on acquisition
const int ACQ_NEXT_STATE = 0;           // Next state to turn on
const uint32_t ACQ_WINDOW_START = 1000; // [ms]
const uint32_t ACQ_WINDOW_END = 2000;   // [ms]

// XBee

XB8XWrapper xbee(&Serial5);

// Create the sensor wrapper objects -------------------------------------------

ADIS16470Wrapper adis16470(CS_ADIS16470_PIN, DR_ADIS16470_PIN,
                           RST_ADIS16470_PIN);
AISx120SXWrapper ais1120sx(CS_AIS1120SX_PIN, _800Hz, _800Hz,
                           false, false, false, false);

HoneywellRscWrapper rscs_0(DR_RSC[0], CS_RS_EE_PIN[0],
                           CS_RSC_ADC_PIN[0], 0,
                           F_DR_2000_SPS, 50000);

/*
HoneywellRscWrapper rscs_1(DR_RSC[1], CS_RS_EE_PIN[1],
                           CS_RSC_ADC_PIN[1], 0,
                           F_DR_2000_SPS, 50000);
*/
MAX31855Wrapper tcs_0(CS_TCS_PIN[0]);
MAX31855Wrapper tcs_1(CS_TCS_PIN[1]);
// MAX31855Wrapper tcs_2(CS_TCS_PIN[2]);
// MAX31855Wrapper tcs_3(CS_TCS_PIN[3]);

AltimaxWrapper altimax(ALTIMAX_DR_PINS[0], ALTIMAX_DR_PINS[1], ALTIMAX_DR_PINS[2]);

MAX7Wrapper max7(&Serial4);

AD8556Wrapper ad8556(255, AD8556_VPLUS, AD8556_VMINUS, 0, 0, 0, ANALOG_READ_RESOLUTION, AD8556_MIN_READ, AD8556_MAX_READ);

// Put all sensors in an array and then all functions can simply loop
// through the array. Requires important overhaul of sensor class and virtual
// functions that are overidden in the derived wrapper classes.
Sensor *sensorArray[NUM_SENSORS] = {&max7,
                                    &adis16470,
                                    &rscs_0,
                                    &ad8556,
                                    &tcs_0,
                                    &tcs_1,
                                    &ais1120sx,
                                    &altimax};

const uint8_t ADIS16470_INDEX = 1,
              AISx120SX_INDEX = 6,
              Honeywell_Rsc_0_INDEX = 2,
              MAX31855_START_INDEX = 4,
              Altimax_INDEX = 7,
              MAX7_INDEX = 0,
              AD8556_INDEX = 1;

const int SENSOR_SETUP_ATTEMPTS = 3;
const int SETUP_DELAY = 200; // delay in ms to wait between setup attemps

// USER FUNCTIONS ==============================================================

// SETUP =======================================================================

void setup()
{
  // Serial communication is started before setup on the Teensy
  delay(5000); // Wait 10s to ensure Serial is ready
  Serial.println("----- Starting setup -----");
  delay(2000); // Wait 5s more to ensure getting Serial feedback

  // Set up I/O
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUTTON0_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT);
  digitalWrite(GREEN_LED_PIN, LOW); // turn off LED in case
  digitalWrite(RED_LED_PIN, LOW);   // turn off LED in case

  Serial.println("[Setup] Play with LEDs");
  delay(500);
  Serial.println("[Setup] Success flash");
  successFlash(); // visual feedback setup is happening
  delay(1000);
  Serial.println("[Setup] Error flash");
  errorFlash();
  delay(1000);

  SPI.begin();
  SPI1.begin();
  // SPI1.setSCK(20);
  Serial4.begin(38400);
  Serial5.begin(115200);

  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogReadAveraging(ANALOG_READ_AVERAGING);

  // buildSensorArray();

  for (size_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.printf("Sensor n° %d: %s ", i, sensorArray[i]->myName());
    if (sensorArray[i]->setup(SENSOR_SETUP_ATTEMPTS, SETUP_DELAY))
    {
      Serial.println("has been set up successfully.");
    }
    else
    {
      Serial.println("could not be set up.");
    }
  }

  Serial.print("XBee ");
  if (xbee.setup(SENSOR_SETUP_ATTEMPTS, SETUP_DELAY))
  {
    Serial.print("has been correctly setup\n");
  }
  else
  {
    Serial.print("could not be setup !!!\n");
  }

  Serial.println("----- Setup complete -----\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  // successFlash();

  acquireData(sensorArray, NUM_SENSORS, SERIAL_PRINT, &xbee);
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
        digitalWrite(GREEN_LED_PIN, LOW);
        successFlash();
        acquireData(sensorArray, NUM_SENSORS, SERIAL_PRINT, &xbee);
        break;
      case BAD_TRANSITION:
        // monitor.writeMessage("Button not pressed properly. Not doing anything.",
        //                      micros(), true);
        digitalWrite(GREEN_LED_PIN, LOW);
        break;
      case WINDOW_START:
        // monitor.writeMessage("Within window to start data acquisition.",
        //                      micros(), true);
        digitalWrite(GREEN_LED_PIN, HIGH);
        break;
      case WINDOW_END:
        // monitor.writeMessage("Left window to start data acquisition.",
        //                      micros(), true);
        digitalWrite(GREEN_LED_PIN, LOW);
        break;
      }
    }

    delay(50);
  }
}