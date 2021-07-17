/*
 * HoneywellRscWrapper.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <Honeywell_RSC.h>

// User-defined headers
#include "Sensor.h"

// packet for both temperature and pressure packets
struct HoneywellRSCPacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float measurement = 0; // 4 bytes

  // constructor for an empty packet
  HoneywellRSCPacket(PacketHeader header_)
  {
    header = header_;
  }

  // constructor
  HoneywellRSCPacket(PacketHeader header_, float measurement_)
  {
    header = header_;
    measurement = measurement_;
  }
};

// packet with the data decoded in floats and appropriately scaled already
struct HoneywellRSCSerialPacket {
  bool errors[ERROR_TYPE_NUM] = {0};
  float pressure;
  float temp;
};

// Wrapper for the AISx120SX class
class HoneywellRscWrapper : public Sensor
{
private:
  // how often temperature measurements will be made
  // every nth measurement will be a temperature measurement
  int temp_frequency;
  int measurementAmountModulo = 0; // modulo of how many measurements were made

  Honeywell_RSC rscObject;
  static uint8_t sensorQty; // how many sensors of this type exist

  HoneywellRSCPacket lastPressurePacket;
  HoneywellRSCPacket lastTempPacket;

public:
  // constructor
  HoneywellRscWrapper(int DR, int CS_EE, int CS_ADC);

  // destructor
  ~HoneywellRscWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration, RSC_DATA_RATE data_rate,
             uint32_t desiredTempPeriod);

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // determine current reading type
  READING_T currReadType();

  // determine next reading type
  READING_T nextReadType();

  HoneywellRSCPacket getPacket(uint32_t currMicros);
  HoneywellRSCSerialPacket getSerialPacket(bool debug = false);
};