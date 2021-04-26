/*
 * Sensor.cpp 
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "Sensor.h"

Sensor::Sensor(uint32_t checkInterval_,
               uint32_t checkIntervalMargin_,
               uint32_t measInterval_,
               bool DR_driven_,
               uint8_t sensorID_) : // initalizer list for constants
                                    CHECK_INTERVAL(checkInterval_),
                                    CHECK_INTERVAL_MARGIN(checkIntervalMargin_),
                                    MEAS_INTERVAL(measInterval_),
                                    DR_DRIVEN(DR_driven_),
                                    SENSOR_ID(sensorID_)
{
  checksumError = false;
}

Sensor::~Sensor()
{
  // do stuff
}

bool Sensor::isDueByTime(uint32_t currMicros)
{
  // check if missed a beat
  if (currMicros - CHECK_INTERVAL_MARGIN - prevCheck > CHECK_INTERVAL)
  {
    // check if skipped some beats
    checkBeatsSkipped = floor((currMicros - prevCheck) / CHECK_INTERVAL);
    if (checkBeatsSkipped > 1)
    {
      Serial.print("WARNING! Skipped following amount of beats:");
      Serial.println(checkBeatsSkipped - 1);
      Serial.println("Consider lowering frequency.");
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

bool Sensor::isDueByDR(uint32_t currMicros, bool currDR, int triggerType)
{
  if (DR_DRIVEN) // make sure the sensor has a data ready line
  {
    // TODO: Find a better rising/falling edge detector
    if (triggerType == RISING && prevDR == 0 && currDR == 1)
    {
      dueMethod = DUE_BY_DR; // sensor is due by DR
      prevCheck = currMicros;
      return true;
    }
    else if (triggerType == FALLING && prevDR == 1 && currDR == 0)
    {
      dueMethod = DUE_BY_DR; // sensor is due by DR
      prevCheck = currMicros;
      return true;
    }
  }
  return false;
}

bool Sensor::isMeasurementLate(uint32_t currMicros)
{
  bool measurementLate = currMicros - prevMeas > 2 * MEAS_INTERVAL;
  return measurementLate;
}

uint8_t Sensor::getErrorCode(uint32_t currMicros)
{
  uint8_t errorCode = 0;

  // first bit: if a measurement beat was missed (measurement beat skipped)
  if (isMeasurementLate(currMicros))
  {
    bitSet(errorCode, 7); // set bit to 1
  }

  // second bit: if the acquisition loop skipped a beat
  if (checkBeatsSkipped > 1)
  {
    bitSet(errorCode, 6); // set bit to 1
  }

  // third bit: if DR pin didn't trigger the read
  if (DR_DRIVEN && dueMethod != DUE_BY_DR)
  {
    bitSet(errorCode, 5); // set bit to 1
  }

  // fourth bit: checksum error
  if (checksumError)
  {
    bitSet(errorCode, 4); // set bit to 1
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
  return header;
}
