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
      int16_t probeT = max31855Object.rawToProbe(rawMeas);
      int16_t ambientT = max31855Object.rawToAmbient(rawMeas);
      if (prevProbeMeas != probeT || prevAmbientMeas != ambientT) // data is new
      {
        prevMeasTime = currMicros;
        returnVal = true;
      }
      // update the last measurements
      prevProbeMeas = probeT;
      prevAmbientMeas = ambientT;
    }
  }
  return returnVal;
}

uint8_t MAX31855Wrapper::getSensorQty()
{
  return sensorQty;
}

MAX31855Packet MAX31855Wrapper::getPacket(uint32_t currMicros)
{
  // create and write the packet
  lastPacket = MAX31855Packet(getHeader(MAX31855_PACKET_TYPE,
                                        sizeof(MAX31855Packet),
                                        currMicros),
                              prevProbeMeas, prevAmbientMeas);
  return lastPacket;
}

serialPacket MAX31855Wrapper::getSerialPacket(bool debug = false)
{
  serialPacket packet;
  packet.errors = getErrors();

  float readings[2];

  if (debug)
  {
    readings[0] = generateFakeData(-200, 1200, micros(), 35, 2700000);
    readings[1] = generateFakeData(-200, 1200, micros(), 25, 8700000);
  }
  else
  {
    readings[0] = lastPacket.probeTemperature;
    readings[1] = lastPacket.sensorTemperature;
  }

  packet.readings = readings;

  return packet;
}