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
#include "ADIS16470Wrapper.hpp"
#include "AISx120SXWrapper.hpp"
#include "HoneywellRscWrapper.hpp"
#include "MAX31855Wrapper.hpp"

// DEFINE VARIABLES ============================================================

#define DEBUG
//const bool DEBUG = false;

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
const uint8_t staticRSC = 0;
const uint8_t totalRSC = 1;
const uint8_t CS_RS_EE_PIN[2] = {17, 28};
const uint8_t CS_RSC_ADC_PIN[2] = {16, 27};
// RSC1's DR line should be on pin 15 according to the PCB, however, it was
// manually reconnected to pin 39 on the first iteration of the PCB
// change lines accordingly if building a new PCB from the schematics
//const uint8_t DR_RSC[2] = {39, 26};
const uint8_t DR_RSC[2] = {15, 26};
// Thermocouples
const uint8_t TAT_TC = 3;
const uint8_t CS_TCS_PIN[4] = {23, 22, 21, 20};
// Altimax
const uint8_t ALTIMAX_DR_PIN = 33;

// I/O -------------------------------------------------------------------------
// Button event
const int ACQ_STATE = 1;                // State to turn on acquisition
const int ACQ_NEXT_STATE = 0;           // Next state to turn on
const uint32_t ACQ_WINDOW_START = 1000; // [ms]
const uint32_t ACQ_WINDOW_END = 2000;   // [ms]

// Create the sensor wrapper objects -------------------------------------------

/* Using Sensor* array instead 
ADIS16470Wrapper adis16470(CS_ADIS16470_PIN, DR_ADIS16470_PIN,
                           RST_ADIS16470_PIN);
AISx120SXWrapper ais1120sx(CS_AIS1120SX_PIN);

HoneywellRscWrapper *rscs[2] = {NULL};
MAX31855Wrapper *tcs[4];
*/

/* TODO : MAKE AN ALTIMAX WRAPPER
// the altimax doesn't have a wrapper because I'm lazy and it's just 1 DR pin
Sensor altimax = Sensor(0);
*/

// TODO: Put all sensors in an array and then all functions can simply loop
// through the array. Requires important overhaul of sensor class and virtual
// functions that are overidden in the derived wrapper classes.
const uint8_t NUM_SENSORS = 9;
Sensor *sensorArray[NUM_SENSORS];

const int SENSOR_SETUP_ATTEMPTS = 7;
const int SETUP_DELAY = 100; // delay in ms to wait between setup attemps

// USER FUNCTIONS ==============================================================

// SETUP =======================================================================

void buildSensorArray()
{
  sensorArray[0] = new ADIS16470Wrapper(CS_ADIS16470_PIN, DR_ADIS16470_PIN,
                                        RST_ADIS16470_PIN);

  sensorArray[1] = new AISx120SXWrapper(CS_AIS1120SX_PIN, _800Hz, _800Hz,
                                        false, false, false, false);

  sensorArray[2] = new HoneywellRscWrapper(DR_RSC[0], CS_RS_EE_PIN[0],
                                           CS_RSC_ADC_PIN[0], 1,
                                           F_DR_2000_SPS, 50000);

  sensorArray[3] = new HoneywellRscWrapper(DR_RSC[1], CS_RS_EE_PIN[1],
                                           CS_RSC_ADC_PIN[1], 0,
                                           F_DR_2000_SPS, 50000);

  for(size_t t = 0; t < 4; t++)
  {
    sensorArray[4+t] = new MAX31855Wrapper(CS_TCS_PIN[t]);
  }

  // TODO:
  // sensorArray[8] = new AltimaxWrapper(args...);                           
}

void setup()
{
  // Serial communication is started before setup on the Teensy
  Serial.println("----- Starting setup -----");

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
  SPI1.begin();
  // SPI1.setSCK(20);

  for(size_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.printf("Sensor n° %d: %s ",i,sensorArray[i]->myName());
    if (sensorArray[i]->setup(SENSOR_SETUP_ATTEMPTS, SETUP_DELAY))
    {
      Serial.println("has been set up successfully.")
    }
    else
    {
      Serial.println("could not be set up.")
    }
  }

  /* TODO : Use a proper wrapper instead
  // Setup the Altimax
  altimax.setupProperties(UINT32_MAX, 0, UINT32_MAX, true);
  pinMode(ALTIMAX_DR_PIN, INPUT_PULLDOWN);
  */

  Serial.println("----- Setup complete -----");
  successFlash();

  acquireData(adis16470, ais1120sx, rscs, tcs, altimax);
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
        successFlash();
        acquireData(adis16470, ais1120sx, rscs, tcs, altimax);
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