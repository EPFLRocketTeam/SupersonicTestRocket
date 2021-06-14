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
                                             true),
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

bool HoneywellRscWrapper::setup(int attempts, int delayDuration,
                                RSC_DATA_RATE data_rate)
{
  // Try to see if the RSC is working
  for (int i = 0; i < attempts; i++)
  {
    rscObject.init(data_rate); // initialize the object
    // try to make a dummy measurement
    rscObject.adc_request(PRESSURE);
    delay(50);
    float reading = rscObject.get_pressure();
    if (reading != 0 && !isnan(reading)) // condition for success
    {
      active = true;
      rscObject.adc_request(nextReadType());
      measurementAmountModulo += 1;
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

bool HoneywellRscWrapper::isDue(uint32_t currMicros, volatile bool &triggeredDR)
{
  if (isDueByDR(currMicros, triggeredDR) || isDueByTime(currMicros))
  {
    prevMeasTime = currMicros;
    return true;
  }
  else
  {
    return false;
  }
}

READING_T HoneywellRscWrapper::currReadType()
{
  // temperature readings are requested when modulo == 0
  // therefore the next measurement (modulo == 1) is a temperature reading
  if (measurementAmountModulo == 1)
  {
    return TEMPERATURE;
  }
  else
  {
    return PRESSURE;
  }
}

READING_T HoneywellRscWrapper::nextReadType()
{
  // temperature readings are requested when modulo == 0
  if (measurementAmountModulo == 0)
  {
    return TEMPERATURE;
  }
  else
  {
    return PRESSURE;
  }
}

HoneywellRSCPacket HoneywellRscWrapper::getPacket(uint32_t currMicros)
{
  float meas;
  packetType packetTypeNum;

  // determine the type of measurement we are getting
  if (currReadType() == TEMPERATURE)
  {
    meas = rscObject.get_temperature(); // get the measurement
    packetTypeNum = RSC_TEMP_PACKET_TYPE; // set the packet type
  }
  else
  {
    meas = rscObject.get_pressure(); // get the measurement
    packetTypeNum = RSC_PRESSURE_PACKET_TYPE; // set the packet type
  }

  // request the next data from the adc
  rscObject.adc_request(nextReadType());

  // update the measurement count
  measurementAmountModulo += 1;
  measurementAmountModulo = measurementAmountModulo % TEMP_FREQUENCY;

  // create and write the packet
  HoneywellRSCPacket packet(getHeader(packetTypeNum,
                                      sizeof(HoneywellRSCPacket),
                                      currMicros),
                            meas);
  return packet;
}