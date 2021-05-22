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
MAX31855Wrapper::MAX31855Wrapper() : Sensor(CHECK_INTERVAL,
                                            MEASUREMENT_MARGIN,
                                            MEASUREMENT_INTERVAL,
                                            false)
{
  sensorID = sensorQty;
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
    if (max31855Object.begin(CS))
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
    if (!(rawMeas & B111)) // if no error
    {
      if (prevMeas != rawMeas) // if data is new
      {
      prevMeasTime = currMicros;
      returnVal = true;
      }
      prevMeas = rawMeas; // update the last measurement
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
  // read the measurements from the sensor
  int32_t rawMeas = max31855Object.readRaw();
  int16_t probeT = max31855Object.rawToProbe(rawMeas);
  int16_t ambientT = max31855Object.rawToAmbient(rawMeas);

  // create and write the packet

  MAX31855Packet packet(getHeader(MAX31855_PACKET_TYPE,
                                  sizeof(MAX31855Packet),
                                  currMicros),
                        probeT, ambientT);
  return packet;
}