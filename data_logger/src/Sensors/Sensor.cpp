/*
 * Sensor.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "Sensor.hpp"

Sensor::Sensor(uint8_t sensorID) : SENSOR_ID(sensorID)
{
  checksumError = false;
}

void Sensor::setupProperties(uint32_t checkInterval_,
                             uint32_t checkIntervalMargin_,
                             uint32_t measInterval_, bool DR_driven_)
{
  CHECK_INTERVAL = checkInterval_;
  CHECK_INTERVAL_MARGIN = checkIntervalMargin_;
  MEAS_INTERVAL = measInterval_;
  DR_DRIVEN = DR_driven_;
}

bool Sensor::isDueByTime(uint32_t currMicros)
{
  // Check if skipped some beats
  checkBeatsSkipped = floor((currMicros - prevCheck) / CHECK_INTERVAL);

  // If the sensor starts reading by time, we remove the margin and simply
  // rely on the nominal checking interval
  // This resets every time the sensor is read by DR (usual behaviour)
  uint32_t actualCheckInterval;

  if (dueMethod == DUE_BY_TIME)
  {
    actualCheckInterval = CHECK_INTERVAL;
  }
  else
  {
    actualCheckInterval = CHECK_INTERVAL + CHECK_INTERVAL_MARGIN;
  }

  
  if (currMicros - prevCheck > actualCheckInterval)
  {
    // Why is prevCheck actualized by incrementing instead of simply taking the actual time???
    // prevCheck += checkBeatsSkipped * CHECK_INTERVAL; // catch up
    prevCheck = currMicros;

    dueMethod = DUE_BY_TIME; // sensor is due by time
    
    return true;             // event is due
  }
  else
  {
    
    return false; // event is not due
  }
}

bool Sensor::isDueByDR(uint32_t currMicros, volatile bool &triggeredDR)
{
  // check if the sensor has a data ready line and it was toggled
  if (DR_DRIVEN && triggeredDR)
  {
    triggeredDR = false;   // reset the interrupt flag
    dueMethod = DUE_BY_DR; // sensor is due by DR
    prevCheck = currMicros;
    return true;
  }
  return false;
}

bool Sensor::isMeasurementLate(uint32_t currMicros)
{
  measurementLate = currMicros - prevMeasTime > 2 * MEAS_INTERVAL;
  return measurementLate;
}

void Sensor::updateErrors(uint32_t currMicros)
{

  errors[0] = isMeasurementLate(currMicros);
  errors[1] = checkBeatsSkipped > 1;
  errors[2] = DR_DRIVEN && dueMethod != DUE_BY_DR;
  errors[3] = checksumError;
  errors[4] = isMeasurementInvalid();
}

// generates a packet header for the sensor
PacketHeader Sensor::getHeader(packetType packetType_, uint8_t packetSize_,
                               uint32_t currMicros)
{
  PacketHeader header;

  updateErrors(currMicros);

  header.packetType_ = packetType_;
  header.packetSize = packetSize_;
  header.sensorID = SENSOR_ID;
  header.errorCode = getErrorCode(errors);
  header.timestamp = currMicros;
  return header;
}

float generateFakeData(float minValue, float maxValue,
                       uint32_t currMicros,
                       float offset, uint32_t period)
{
  float mid = (minValue + maxValue) / 2.;
  float amplitude = (maxValue - minValue) / 2.;

  return sin(2 * PI * currMicros / period) * amplitude + mid + offset;
}