/*
 * HoneywellRscWrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "HoneywellRscWrapper.h"

// initialize the sensor count
uint8_t HoneywellRscWrapper::sensorQty = 0;

// constructor
HoneywellRscWrapper::
    HoneywellRscWrapper(int DR,
                        int CS_EE,
                        int CS_ADC) : Sensor(CHECK_INTERVAL,
                                             MEASUREMENT_MARGIN,
                                             MEASUREMENT_INTERVAL,
                                             false),
                                      rscObject(DR, CS_EE, CS_ADC)
{
  sensorID = sensorQty;
  sensorQty += 1;
}

// destructor
HoneywellRscWrapper::~HoneywellRscWrapper()
{
  sensorQty -= 1;
}

bool HoneywellRscWrapper::setup(int attempts, int delayDuration)
{
  // Try to see if the RSC is working
  for (int i = 0; i < attempts; i++)
  {
    if (true) // condition for success
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

uint8_t HoneywellRscWrapper::getSensorQty()
{
  return sensorQty;
}

bool HoneywellRscWrapper::isDue(uint32_t currMicros, bool currDR)
{
  if (isDueByDR(currMicros, currDR, RISING) || isDueByTime(currMicros))
  {
    prevMeasTime = currMicros;
    return true;
  }
  else
  {
    return false;
  }
}

HoneywellRSCPacket HoneywellRscWrapper::getPacket(uint32_t currMicros)
{
  // read the measurements from the sensor
  float presure = rscObject.get_pressure();

  // create and write the packet

  HoneywellRSCPacket packet(getHeader(RSC_PRESSURE_PACKET_TYPE,
                                      sizeof(HoneywellRSCPacket),
                                      currMicros),
                            presure);
  return packet;
}