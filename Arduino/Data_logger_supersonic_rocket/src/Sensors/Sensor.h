/*
 * Sensor.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include <Arduino.h>

class Sensor
{
private:
  uint32_t prevCheck;           // when the sensor was last checked for data
  uint32_t prevMeas;            // when the last new measurement was made
  const uint32_t checkInterval; // how often to check for data
  const uint32_t measInterval;  // nominal interval for data measurement

public:
}