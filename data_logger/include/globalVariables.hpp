#pragma once

#include <Arduino.h>

// debugging mode
// for now it's just whether to take real data or generate some fake one

//#define DEBUG
const bool SERIAL_PRINT = true;
//#include "TeensyDebug.h"
//#pragma GCC optimize ("O0")

// Experiment duration, in ms (set to 1h)
#define EXPERIMENT_DURATION 1*3600*1000

// Pinout ----------------------------------------------------------------------
// I/O
extern const uint8_t GREEN_LED_PIN,
    RED_LED_PIN,
    BUTTON0_PIN,
    BUTTON1_PIN;
// ADIS164760
extern const uint8_t DR_ADIS16470_PIN,
    SYNC_ADIS16470_PIN,
    RST_ADIS16470_PIN,
    CS_ADIS16470_PIN;
// AIS1120SX
extern const uint8_t CS_AIS1120SX_PIN;
// Pressure sensors
extern const uint8_t staticRSC;
extern const uint8_t totalRSC;
extern const uint8_t CS_RS_EE_PIN[2],
    CS_RSC_ADC_PIN[2],
    DR_RSC[2];
// Thermocouples
extern const uint8_t TAT_TC;
extern const uint8_t CS_TCS_PIN[4];
// Altimax
extern const uint8_t ALTIMAX_DR_PINS[3];

// XBee
extern const uint8_t XBEE_PINS[];

// MAX-7
extern const uint8_t MAX7_PINS[];

// Sensors ----------------------------------------------------------------------

constexpr size_t NUM_SENSORS = 8; //Should hold in uint8_t

extern const uint8_t ADIS16470_INDEX,
    AISx120SX_INDEX,
    Honeywell_Rsc_0_INDEX,
    Honeywell_Rsc_1_INDEX,
    MAX31855_START_INDEX,
    Altimax_INDEX,
    MAX7_INDEX,
    AD8556_INDEX;