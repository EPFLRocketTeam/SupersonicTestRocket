#pragma once

#include <Arduino.h>

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
extern const uint8_t CS_RS_EE_PIN[2],
    CS_RSC_ADC_PIN[2],
    DR_RSC[2];
// Thermocouples
extern const uint8_t CS_TCS_PIN[4];