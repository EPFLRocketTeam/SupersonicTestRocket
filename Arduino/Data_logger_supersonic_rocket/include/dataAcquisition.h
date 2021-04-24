/*
 * dataAcquisition.h
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>

// SdFat
//    Available at: https://github.com/greiman/SdFat
//    Also available from the library manager.
#include <SdFat.h>
#include <RingBuf.h>

// sensors
#include <ADIS16470.h>
#include <AISx120SX.h>
#include <Honeywell_RSC.h>
#include <MAX31855.h>

// user headers
#include "globalVariables.h"
#include "logging.h"
#include "PushButtonArray/PushButtonArray.h"
#include "io.h"

// Button events settings
const int STOP_STATES[3] = {1, 0, 2};                          // States to stop
const int NEXT_STOP_STATES[3] = {0, 2, 0};                     // Next states
const unsigned long STOP_WINDOW_START[3] = {3000, 5000, 7000}; // [ms]
const unsigned long STOP_WINDOW_END[3] = {4000, 6000, 8000};   // [ms]

// Acquisition intervals
const int ADIS16470_INTERVAL = 500;     // [us] (2000Hz)
const int ADIS16470_MARGIN = 50;        // [us]
const int HONEYWELL_RSC_INTERVAL = 520; // [us] (~2000Hz)
const int HONEYWELL_RSC_MARGIN = 50;    // [us]
const int AIS1120SX_INTERVAL = 2500;    // [us] (400Hz)
const int TC_INTERVAL = 100 * 1000;     // [us] (100 ms)
const int SYNC_INTERVAL = 100 * 1000;   // [us] (100 ms)

// Start to acquire data periodically from the sensors and log it in a file
// Sets up the necessary things and then loops periodically
void acquireData(ADIS16470 adis16470, AISx120SX ais1120sx,
                 Honeywell_RSC rsc015, Honeywell_RSC rsc060,
                 MAX31855_Class tcs[4]);


// Checks if the event is due and if it has skipped any beats
int8_t checkEventDue(unsigned long currMicros, unsigned long &prevEvent,
                     unsigned long interval);

// checkEventDueTime
// checkEventDueDRPin

// check and acquire data from the ADIS16470
//void acquireAdis16470()

void flashLED(uint8_t pinNumber, int flashDuration);

// Check the buttons and see if loop should continue according to input.
bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3]);