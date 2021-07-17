/*
 * Sensor.cpp 
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "Sensor.h"

Sensor::Sensor(uint8_t sensorID) : SENSOR_ID(sensorID)
{
  checksumError = false;
}

Sensor::~Sensor()
{
  // do stuff
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
  // check if skipped some beats
  checkBeatsSkipped = floor((currMicros - prevCheck) / CHECK_INTERVAL);

  uint32_t actualCheckInterval = CHECK_INTERVAL + CHECK_INTERVAL_MARGIN;
  // if the sensor starts reading by time, we remove the margin and simply
  // rely on the nominal checking interval
  // this resets every time the sensor is read by DR (usual behaviour)
  if (dueMethod == DUE_BY_TIME)
  {
    actualCheckInterval = CHECK_INTERVAL;
  }
  if (currMicros - prevCheck > actualCheckInterval)
  {
    if (checkBeatsSkipped > 1)
    {
      // Serial.print("WARNING! Skipped following amount of beats:");
      // Serial.println(checkBeatsSkipped - 1);
      // Serial.println("Consider lowering frequency.");
    }
    prevCheck += checkBeatsSkipped * CHECK_INTERVAL; // catch up
    dueMethod = DUE_BY_TIME;                         // sensor is due by time
    return true;                                     // event is due
  }
  else
  {
    return 0; // event is not due
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

// if the measurement was late. should be redefined in inherited classes
// to properly define what it means to be invalid
bool Sensor::isMeasurementInvalid()
{
  return false;
}

bool *Sensor::getErrors()
{
  bool errors[ERROR_TYPE_NUM] = {false}; // array of flags of errors that occured

  // first error: if a measurement beat was missed (measurement beat skipped)
  if (measurementLate)
  {
    errors[0] = true;
  }

  // second error: if the acquisition loop skipped a beat
  if (checkBeatsSkipped > 1)
  {
    errors[1] = true;
  }

  // third error: if DR pin didn't trigger the read
  if (DR_DRIVEN && dueMethod != DUE_BY_DR)
  {
    errors[2] = true;
  }

  // fourth error: checksum error
  if (checksumError)
  {
    errors[3] = true;
  }

  // fifth error: invalid measurement
  if (isMeasurementInvalid())
  {
    errors[4] = false;
  }

  return errors;
}

uint8_t Sensor::getErrorCode(uint32_t currMicros)
{
  uint8_t errorCode = 0;

  isMeasurementLate(currMicros); // update the measurementLate variable
  bool *errors = getErrors();

  for (size_t i = 0; i < sizeof(errors); i++)
  {
    if (errors[i])
    {
      bitSet(errorCode, 7 - i);
    }
  }

  return errorCode;
}

// generates a packet header for the sensor
PacketHeader Sensor::getHeader(packetType packetType_, uint8_t packetSize_,
                               uint32_t currMicros)
{
  PacketHeader header;
  header.packetType_ = packetType_;
  header.packetSize = packetSize_;
  header.sensorID = SENSOR_ID;
  header.errorCode = getErrorCode(currMicros);
  header.timestamp = currMicros;
  return header;
}

float Sensor::generateFakeData(float minValue, float maxValue,
                               uint32_t currMicros,
                               float offset = 0, uint32_t period = 5000000)
{
  float mid = (minValue + maxValue) / 2.;
  float amplitude = (maxValue - minValue) / 2.;

  return sin(2 * PI * currMicros / period) * amplitude + mid + offset;
}