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
AISx120SXWrapper::AISx120SXWrapper(uint8_t CS_,
                                   bandwidth bwd_X, bandwidth bwd_Y,
                                   bool x_offset_m, bool x_offset_c,
                                   bool y_offset_m, bool y_offset_c)
    : Sensor(sensorQty),
      aisObject(CS_),
      lastPacket(getHeader(0)),
      bandwidthX(bwd_X),
      bandwidthY(bwd_Y),
      x_offset_monitor(x_offset_m),
      y_offset_monitor(y_offset_m),
      x_offset_canc(x_offset_c),
      y_offset_canc(y_offset_c)
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

bool AISx120SXWrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
  // Try to see if the AIS is working
  for (uint32_t i = 0; i < attempts; i++)
  {
    if (aisObject.setup(bandwidthX, bandwidthY, x_offset_monitor, x_offset_canc,
                        y_offset_monitor, y_offset_canc)) // condition for success
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

bool AISx120SXWrapper::isDue(uint32_t currMicros, unused(volatile bool &triggeredDR))
{
  bool returnVal = false;
  if (isDueByTime(currMicros))
  {
    // read the measurements from the sensor
    int16_t *rawMeas = aisObject.readAccel();

    if (lastPacket.getXaccel() != rawMeas[0] * SENSITIVITY ||
        lastPacket.getYaccel() != rawMeas[1] * SENSITIVITY) // data is new
    {
      returnVal = true;
      prevMeasTime = currMicros;
    }
    // copy new measurements into the old ones
    lastPacket.setXaccel(rawMeas[0] * SENSITIVITY);
    lastPacket.setYaccel(rawMeas[1] * SENSITIVITY);

    lastPacket.updateHeader(getHeader(currMicros));
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

AISx120SXPacket *AISx120SXWrapper::getPacket()
{
#ifdef DEBUG
  lastPacket.setXaccel(generateFakeData(-120, 120, micros()));
  lastPacket.setYaccel(generateFakeData(-120, 120, micros(), 1, 5800000));
// when not debugging readings are updated in isDue()
#endif

  return &lastPacket;
}

PacketHeader AISx120SXWrapper::getHeader(uint32_t currMicros)
{
  return Sensor::getHeader(AISx120SX_PACKET_TYPE,
                           sizeof(AISx120SXBody),
                           currMicros);
}