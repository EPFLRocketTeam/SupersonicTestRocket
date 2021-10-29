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
#include "Sensor.hpp"

// packet for both temperature and pressure packets
struct HoneywellRSCPacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float measurement = 0; // 4 bytes

  // empty constructor
  HoneywellRSCPacket()
  {
    // do nothing
  }

  // constructor for a packet with only a header
  HoneywellRSCPacket(PacketHeader header_)
  {
    header = header_;
  }
};

// Wrapper for the AISx120SX class
class HoneywellRscWrapper : public Sensor
{
private:
  // how often temperature measurements will be made
  // every nth measurement will be a temperature measurement
  int temp_frequency;
  int measurementAmountModulo = 0; // modulo of how many measurements were made

  // sensor min/max values for error checking
  float pressureMax = 15;
  float pressureMin = 0;
  static constexpr float TEMP_MAX = 85;
  static constexpr float TEMP_MIN = -40;

  Honeywell_RSC rscObject;
  static uint8_t sensorQty; // how many sensors of this type exist

  HoneywellRSCPacket lastPressurePacket;
  HoneywellRSCPacket lastTempPacket;

public:
  // constructor
  HoneywellRscWrapper(int DR, int CS_EE, int CS_ADC, int SPI_BUS);

  // destructor
  ~HoneywellRscWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration, RSC_DATA_RATE data_rate,
             uint32_t desiredTempPeriod);

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  // determine current reading type
  READING_T currReadType();

  // determine next reading type
  READING_T nextReadType();

  HoneywellRSCPacket getPacket(uint32_t currMicros);
  HoneywellRSCPacket *getSerialPackets(uint32_t currMicros);
};