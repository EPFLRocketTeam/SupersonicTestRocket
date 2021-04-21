#pragma once

#include <Arduino.h>

// Pinout ----------------------------------------------------------------------
// I/O
extern const byte LED1_PIN,
    LED2_PIN,
    BUTTON0_PIN,
    BUTTON1_PIN;
// ADIS164760
extern const byte DR_ADIS16470_PIN,
    SYNC_ADIS16470_PIN,
    RST_ADIS16470_PIN,
    CS_ADIS16470_PIN;
// AIS1120SX
extern const byte CS_AIS1120SX_PIN;
// Pressure sensors
extern const byte CS_RSC015_EE_PIN,
    CS_RSC015_ADC_PIN,
    DR_RSC015;
extern const byte CS_RSC060_EE_PIN,
    CS_RSC060_ADC_PIN,
    DR_RSC060;
// Thermocouples
extern const byte CS_TC1_PIN,
    CS_TC2_PIN,
    CS_TC3_PIN,
    CS_TC4_PIN;