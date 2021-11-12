/*
 * AISx120SXWrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "AISx120SXWrapper.hpp"

// initialize the sensor count
uint8_t AISx120SXWrapper::sensorQty = 0;

// constructor
AISx120SXWrapper::AISx120SXWrapper(uint8_t CS_) : Sensor(sensorQty),
                                                  aisObject(CS_),
                                                  lastPacket(getHeader(0))
{
  setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN, MEASUREMENT_INTERVAL,
                  false);
  sensorQty += 1;
  active = false;
}

// destructor
AISx120SXWrapper::~AISx120SXWrapper()
{
  sensorQty -= 1;
}

bool AISx120SXWrapper::setup(int attempts, int delayDuration,
                             bandwidth bandwidthX, bandwidth bandwidthY,
                             bool x_offset_monitor, bool x_offset_canc,
                             bool y_offset_monitor, bool y_offset_canc)
{
  // Try to see if the AIS is working
  for (int i = 0; i < attempts; i++)
  {
    if (aisObject.setup(bandwidthX, bandwidthY, x_offset_monitor, x_offset_canc,
                        y_offset_monitor, y_offset_canc)) // condition for success
    {
      active = true;
      return active;
    }
    else // give it time before the next try
    {
      aisObject.reset();
      delay(delayDuration);
    }
  }
  active = false;
  return active; // setup was not succesful
}

uint8_t AISx120SXWrapper::getSensorQty()
{
  return sensorQty;
}

bool AISx120SXWrapper::isDue(uint32_t currMicros)
{
  bool returnVal = false;
  if (isDueByTime(currMicros))
  {
    // read the measurements from the sensor
    int16_t *rawMeas;
    rawMeas = aisObject.readAccel();

    if (lastPacket.getXaccel() != rawMeas[0] * SENSITIVITY ||
        lastPacket.getYaccel() != rawMeas[1] * SENSITIVITY) // data is new
    {
      returnVal = true;
      prevMeasTime = currMicros;
    }
    // copy new measurements into the old ones
    lastPacket.setXaccel(rawMeas[0] * SENSITIVITY);
    lastPacket.setYaccel(rawMeas[1] * SENSITIVITY);
  }
  return returnVal;
}

bool AISx120SXWrapper::isMeasurementInvalid()
{
  if (lastPacket.getXaccel() > ACC_MAX || lastPacket.getXaccel() < ACC_MIN ||
      lastPacket.getYaccel() > ACC_MAX || lastPacket.getYaccel() < ACC_MIN)
  {
    return true;
  }
  return false;
}

AISx120SXPacket AISx120SXWrapper::getPacket(uint32_t currMicros)
{
  // update the error on the packet
  lastPacket.updateHeader(getHeader(currMicros));
#ifdef DEBUG
  lastPacket.accel[0] = generateFakeData(-120, 120, micros());
  lastPacket.accel[1] = generateFakeData(-120, 120, micros(), 1, 5800000);
// when not debugging readings are updated in isDue()
#endif

  return lastPacket;
}

PacketHeader AISx120SXWrapper::getHeader(uint32_t currMicros)
{
  return Sensor::getHeader(AISx120SX_PACKET_TYPE,
                           sizeof(AISx120SXBody),
                           currMicros);
}