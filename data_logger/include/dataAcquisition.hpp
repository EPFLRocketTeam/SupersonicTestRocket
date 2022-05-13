/*
 * dataAcquisition.hpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>
//#include <algorithm>

// SdFat
//    Available at: https://github.com/greiman/SdFat
//    Also available from the library manager.
#include <SdFat.h>
#include <RingBuf.h>

// user headers
#include "globalVariables.hpp"
#include "logging.hpp"
#include "PushButtonArray.h"
#include "io.hpp"

#include "Packet.hpp"
#include "Sensors/Sensor.hpp"
#include "XB8XWrapper.hpp"

// Button events settings
const int STOP_STATES[3] = {1, 0, 2};                     ///< States to stop
const int NEXT_STOP_STATES[3] = {0, 2, 0};                ///< Next states
const uint32_t STOP_WINDOW_START[3] = {3000, 5000, 7000}; ///< [ms]
const uint32_t STOP_WINDOW_END[3] = {4000, 6000, 8000};   ///< [ms]

// Acquisition intervals
const int HONEYWELL_RSC_INTERVAL = 520;                           ///< [us] (~2000 Hz)
const int HONEYWELL_RSC_MARGIN = 50;                              ///< [us]
const int AIS1120SX_NOM_INTERVAL = 2500;                          ///< [us] (400 Hz)
const int AIS1120SX_CHECK_INTERVAL = AIS1120SX_NOM_INTERVAL / 10; ///< [us]
const int SYNC_INTERVAL = 100 * 1000;                             ///< [us] (100 ms)
const int SERIAL_INTERVAL = 20 * 1000;                            ///< [us] (50 Hz)
const int RADIO_INTERVAL = 500 * 1000 * 1;                            //< (500 ms)

// Initialisation for data acquisition
//void initializeDataAcquisition(SdFs *sd, FsFile *loggingFile, RingBuf<FsFile, RING_BUF_CAPACITY> *rb);

// Start to acquire data periodically from the sensors and log it in a file
// Sets up the necessary things and then loops periodically
void acquireData(Sensor *sArray[], size_t sSize, bool serialOutput, XB8XWrapper *xbee);

// Check the buttons and see if loop should continue according to input.
bool checkButtons(PushButtonArray &buttonArray, uint8_t stopEvent[3]);