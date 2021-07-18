/*
 * flowCalculations.h
 *
 *  Created on: 2021-07-18
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>

const float GAMMA = 1.4;                // heat capacity ratio
const float R = 287;                    // gas constant
const float CELSIUS_TO_KELVIN = 273.15; // conversion factor from C to K
const float PSI_TO_PA = 6894.76;        // conversion factor from PSI to Pa
const float ALTITUDE_CONSTANT = 5.14892218924277e-5;
const float STANDARD_PRESSURE = 101325; // pressure at sea level

// isentropic relations
float calcMachFromPressure(float staticPressure, float totalPressure);
float calcStaticTemperature(float mach, float totalTemperature);
float calcAirspeed(float mach, float staticTemperature);
float calcAltitude(float pressure);
