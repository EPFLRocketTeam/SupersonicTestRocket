/*
 * ADIS16470Wrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "MAX31855Wrapper.hpp"

// initialize the sensor count
uint8_t MAX31855Wrapper::sensorQty = 0;

// constructor
MAX31855Wrapper::MAX31855Wrapper(uint8_t cs)
    : Sensor(sensorQty),
      lastPacket(getHeader(0)),
      CS(cs)
{
  setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN,
                  MEASUREMENT_INTERVAL, false);
  sensorQty += 1;
  active = false;
}

// destructor
MAX31855Wrapper::~MAX31855Wrapper()
{
  sensorQty -= 1;
}

bool MAX31855Wrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
  // Try to see if the MAX is working
  for (uint32_t i = 0; i < attempts; i++)
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

bool MAX31855Wrapper::isDue(uint32_t currMicros, volatile bool &triggeredDR)
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
      float ambientT = max31855Object.rawToAmbient(rawMeas) * AMBIENT_SENSITIVITY;

      if (lastPacket.getProbeTemperature() != probeT ||
          lastPacket.getSensorTemperature() != ambientT) // data is new
      {
        prevMeasTime = currMicros;
        returnVal = true;
      }

      // update the last measurements
      lastPacket.setProbeTemperature(probeT);
      lastPacket.setSensorTemperature(ambientT);
    }
  }
  return returnVal;
}

bool MAX31855Wrapper::isMeasurementInvalid()
{
  if (lastPacket.getProbeTemperature() > PROBE_MAX ||
      lastPacket.getProbeTemperature() < PROBE_MIN ||
      lastPacket.getSensorTemperature() > AMBIENT_MAX ||
      lastPacket.getSensorTemperature() < AMBIENT_MIN)
  {
    return true;
  }
  return false;
}

MAX31855Packet* MAX31855Wrapper::getPacket(uint32_t currMicros)
{
  // update the error on the packet
  lastPacket.updateHeader(getHeader(currMicros));

#ifdef DEBUG

  lastPacket.probeTemperature =
      generateFakeData(-200, 1200, micros(), 35 * SENSOR_ID, 2700000);
  lastPacket.sensorTemperature =
      generateFakeData(-200, 1200, micros(), 25 * SENSOR_ID, 8700000);

// when not debugging readings are updated in isDue()
#endif

  return &lastPacket;
}

PacketHeader MAX31855Wrapper::getHeader(uint32_t currMicros)
{
  return Sensor::getHeader(MAX31855_PACKET_TYPE,
                           sizeof(MAX31855Body),
                           currMicros);
}