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
                        int CS_ADC) : Sensor(sensorQty),
                                      rscObject(DR, CS_EE, CS_ADC),
                                      lastPressurePacket(getHeader(
                                          RSC_PRESSURE_PACKET_TYPE,
                                          sizeof(HoneywellRSCPacket),
                                          0)),
                                      lastTempPacket(getHeader(
                                          RSC_TEMP_PACKET_TYPE,
                                          sizeof(HoneywellRSCPacket),
                                          0))
{
  sensorQty += 1;
}

// destructor
HoneywellRscWrapper::~HoneywellRscWrapper()
{
  sensorQty -= 1;
}

bool HoneywellRscWrapper::setup(int attempts, int delayDuration,
                                RSC_DATA_RATE data_rate,
                                uint32_t desiredTempPeriod)
{
  // Setup the timing parameters

  // Try to see if the RSC is working
  for (int i = 0; i < attempts; i++)
  {
    rscObject.init(data_rate); // initialize the object

    pressureMin = rscObject.pressure_minimum();
    pressureMax = rscObject.pressure_range() + pressureMin;
    // try to make a dummy measurement
    rscObject.adc_request(PRESSURE);
    delay(50);
    float reading = rscObject.get_pressure();
    if (reading != 0 && !isnan(reading)) // condition for success
    {
      active = true;
      temp_frequency = floor(desiredTempPeriod / rscObject.calc_dr_delay());
      setupProperties(rscObject.calc_dr_delay(), rscObject.calc_dr_delay(),
                      rscObject.calc_dr_delay(), true);

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

bool HoneywellRscWrapper::isMeasurementInvalid()
{
  if (lastSerialPacket.pressure > pressureMax ||
      lastSerialPacket.pressure < pressureMin ||
      lastSerialPacket.temp > TEMP_MAX ||
      lastSerialPacket.temp < TEMP_MIN)
  {
    return true;
  }
  return false;
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
    packetTypeNum = RSC_TEMP_PACKET_TYPE; // set the packet type
    lastTempPacket = HoneywellRSCPacket(getHeader(packetTypeNum,
                                                  sizeof(HoneywellRSCPacket),
                                                  currMicros),
                                        rscObject.get_temperature());

    // request the next data from the adc
    rscObject.adc_request(nextReadType());

    // update the measurement count
    measurementAmountModulo += 1;
    measurementAmountModulo = measurementAmountModulo % temp_frequency;
    return lastTempPacket;
  }
  else
  {
    packetTypeNum = RSC_PRESSURE_PACKET_TYPE; // set the packet type
    lastPressurePacket = HoneywellRSCPacket(
        getHeader(packetTypeNum,
                  sizeof(HoneywellRSCPacket),
                  currMicros),
        rscObject.get_pressure());

    // request the next data from the adc
    rscObject.adc_request(nextReadType());

    // update the measurement count
    measurementAmountModulo += 1;
    measurementAmountModulo = measurementAmountModulo % temp_frequency;
    return lastPressurePacket;
  }
}

HoneywellRSCSerialPacket HoneywellRscWrapper::getSerialPacket(bool debug = false)
{
  if (debug)
  {
    lastSerialPacket.pressure =
        generateFakeData(0, 2, micros(), 14 * SENSOR_ID);
    lastSerialPacket.temp =
        generateFakeData(-200, 1200, micros(), 25 * SENSOR_ID, 3800000);
  }
  else
  {
    lastSerialPacket.pressure = lastPressurePacket.measurement;
    lastSerialPacket.temp = lastTempPacket.measurement;
  }
  memcpy(lastSerialPacket.errors, getErrors(), ERROR_TYPE_NUM);

  return lastSerialPacket;
}
