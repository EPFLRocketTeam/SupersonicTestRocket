/*
 * AISx120SXWrapper.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <AISx120SX.h>

// User-defined headers
#include "Sensor.h"

struct AISx120SXPacket
{
  struct PacketHeader header; // 8 bytes
  float accel[2] = {0};           // 4 bytes
  // following unused for the AIS1120SX but included for AIS2120SX
  //float accelY = 0; // 4 bytes

  // constructor for an empty packet
  AISx120SXPacket(PacketHeader header_)
  {
    header = header_;
  }
};

// Wrapper for the AISx120SX class
class AISx120SXWrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 1250; // [us] (800 Hz)
  static const uint32_t CHECK_INTERVAL =
      MEASUREMENT_INTERVAL / 2;                 // [us]
  static const uint32_t MEASUREMENT_MARGIN = 0; // [us]

  // sensor properties for error checking and conversions
  static constexpr float SENSITIVITY = 2;//1 / (68 * 4.); // [g/LSB]
  static constexpr float ACC_MAX = 120;
  static constexpr float ACC_MIN = -120;

  AISx120SX aisObject;
  static uint8_t sensorQty; // how many sensors of this type exist

  AISx120SXPacket lastPacket;

public:
  // constructor
  AISx120SXWrapper(uint8_t CS_);

  // destructor
  ~AISx120SXWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration,
             bandwidth bandwidthX, bandwidth bandwidthY,
             bool x_offset_monitor, bool x_offset_canc,
             bool y_offset_monitor, bool y_offset_canc);

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  AISx120SXPacket getPacket(uint32_t currMicros, bool debug);
};