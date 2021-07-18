/*
 * flowCalculations.cpp
 *
 *  Created on: 2021-07-18
 *      Author: Joshua Cayetano-Emond
 */

#include "flowCalculations.h"

float calcMachFromPressure(float staticPressure, float totalPressure)
{
  float mach = sqrt((2 / (GAMMA - 1)) * (pow(staticPressure / totalPressure,
                                             (1 - GAMMA) / GAMMA) -
                                         1));
  return mach;
}

float calcStaticTemperature(float mach, float totalTemperature)
{
  float staticTemperature = (totalTemperature + CELSIUS_TO_KELVIN) /
                            (1 + (GAMMA - 1) / 2 * pow(mach, 2));

  return staticTemperature;
}

float calcAirspeed(float mach, float staticTemperature)
{
  float speedOfSound = sqrt(GAMMA * R * staticTemperature);
  float airspeed = speedOfSound * mach;

  return airspeed;
}

float calcAltitude(float pressure)
{
  float altitude = - log(pressure / STANDARD_PRESSURE) / ALTITUDE_CONSTANT;
  
  return altitude;
}