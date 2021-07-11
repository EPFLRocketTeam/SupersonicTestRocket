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

// user headers
#include "globalVariables.h"
#include "logging.h"
#include "PushButtonArray.h"
#include "io.h"
#include "serialOutput.h"
//    Sensors
#include "ADIS16470Wrapper.h"
#include "AISx120SXWrapper.h"
#include "HoneywellRscWrapper.h"
#include "MAX31855Wrapper.h"

// Button events settings
const int STOP_STATES[3] = {1, 0, 2};                     // States to stop
const int NEXT_STOP_STATES[3] = {0, 2, 0};                // Next states
const uint32_t STOP_WINDOW_START[3] = {3000, 5000, 7000}; // [ms]
const uint32_t STOP_WINDOW_END[3] = {4000, 6000, 8000};   // [ms]

// Acquisition intervals
const int HONEYWELL_RSC_INTERVAL = 520;  // [us] (~2000 Hz)
const int HONEYWELL_RSC_MARGIN = 50;     // [us]
const int AIS1120SX_NOM_INTERVAL = 2500; // [us] (400 Hz)
const int AIS1120SX_CHECK_INTERVAL =
    AIS1120SX_NOM_INTERVAL / 10;       // [us]
const int SYNC_INTERVAL = 100 * 1000;  // [us] (100 ms)
const int SERIAL_INTERVAL = 20 * 1000; // [us] (50 Hz)

// Start to acquire data periodically from the sensors and log it in a file
// Sets up the necessary things and then loops periodically
void acquireData(ADIS16470Wrapper adis16470, AISx120SXWrapper ais1120sx,
                 HoneywellRscWrapper *rscs, MAX31855Wrapper *tcs,
                 Sensor altimax);

// Check the buttons and see if loop should continue according to input.
bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3]);