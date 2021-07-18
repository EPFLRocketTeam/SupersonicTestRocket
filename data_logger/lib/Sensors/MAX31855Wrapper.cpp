/*
 * ADIS16470Wrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "MAX31855Wrapper.h"

// initialize the sensor count
uint8_t MAX31855Wrapper::sensorQty = 0;

// constructor
MAX31855Wrapper::MAX31855Wrapper() : Sensor(sensorQty),
                                     lastPacket(getHeader(
                                         MAX31855_PACKET_TYPE,
                                         sizeof(MAX31855Packet),
                                         0))
{
  setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN,
                  MEASUREMENT_INTERVAL, false);
  sensorQty += 1;
}

// destructor
MAX31855Wrapper::~MAX31855Wrapper()
{
  sensorQty -= 1;
}

bool MAX31855Wrapper::setup(int attempts, int delayDuration, uint8_t CS)
{
  // Try to see if the MAX is working
  for (int i = 0; i < attempts; i++)
  {
    if (!max31855Object.begin(CS))
    {
      active = true;
      return active;
    }
    else // give it time before the next try
    {
      delay(delayDuration);
    }
  }
  active = false;
  return active; // setup was not succesful
}

uint8_t MAX31855Wrapper::getSensorQty()
{
  return sensorQty;
}

bool MAX31855Wrapper::isDue(uint32_t currMicros)
{
  bool returnVal = false;
  if (isDueByTime(currMicros))
  {
    // read the measurements from the sensor
    int32_t rawMeas;
    rawMeas = max31855Object.readRaw();
    if (!max31855Object.fault()) // if no error
    {
      float probeT = max31855Object.rawToProbe(rawMeas) * PROBE_SENSITIVITY;
      float ambientT = max31855Object.rawToAmbient(rawMeas) *
                       AMBIENT_SENSITIVITY;
      if (lastPacket.probeTemperature != probeT ||
          lastPacket.sensorTemperature != ambientT) // data is new
      {
        prevMeasTime = currMicros;
        returnVal = true;
      }
      // update the last measurements
      lastPacket.probeTemperature = probeT;
      lastPacket.sensorTemperature = ambientT;
    }
  }
  return returnVal;
}

bool MAX31855Wrapper::isMeasurementInvalid()
{
  if (lastPacket.probeTemperature > PROBE_MAX ||
      lastPacket.probeTemperature < PROBE_MIN ||
      lastPacket.sensorTemperature > AMBIENT_MAX ||
      lastPacket.sensorTemperature < AMBIENT_MIN)
  {
    return true;
  }
  return false;
}

MAX31855Packet MAX31855Wrapper::getPacket(uint32_t currMicros, bool debug)
{
  // update the error on the packet
  lastPacket.header = getHeader(MAX31855_PACKET_TYPE,
                                sizeof(MAX31855Packet),
                                currMicros);
                               
  if (debug)
  {
    lastPacket.probeTemperature =
        generateFakeData(-200, 1200, micros(), 35 * SENSOR_ID, 2700000);
    lastPacket.sensorTemperature =
        generateFakeData(-200, 1200, micros(), 25 * SENSOR_ID, 8700000);
  }
  // when not debugging readings are updated in isDue()

  return lastPacket;
}