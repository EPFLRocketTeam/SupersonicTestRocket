/*
 * MAX31855Wrapper.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <MAX31855.h>

// User-defined headers
#include "Sensor.hpp"

struct MAX31855Packet
{
  struct PacketHeader header;  // 8 bytes
  float probeTemperature = 0;  // 4 bytes
  float sensorTemperature = 0; // 4 bytes

  // constructor for an empty packet
  MAX31855Packet()
  {
    // do nothing
  }

  // constructor for an packet with only header
  MAX31855Packet(PacketHeader header_)
  {
    header = header_;
  }
};

// Wrapper for the MAX31855 class
class MAX31855Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 70 * 1000; // [us] (70 ms)
  static const uint32_t CHECK_INTERVAL =
      MEASUREMENT_INTERVAL / 10;                // [us] (2000Hz)
  static const uint32_t MEASUREMENT_MARGIN = 0; // [us]

  // sensor properties for error checking and conversions
  static constexpr float PROBE_SENSITIVITY = 0.25 / 4.; // [degC/LSB]
  static constexpr float PROBE_MAX = 1372;
  static constexpr float PROBE_MIN = -270;
  static constexpr float AMBIENT_SENSITIVITY = 0.0625 / 16.; // [degC/LSB]
  static constexpr float AMBIENT_MAX = 125;
  static constexpr float AMBIENT_MIN = -55;

  MAX31855_Class max31855Object;
  static uint8_t sensorQty; // how many sensors of this type exist

  // previous measurements from the sensor
  MAX31855Packet lastPacket;

public:
  // constructor
  MAX31855Wrapper();

  // destructor
  ~MAX31855Wrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration, uint8_t CS);

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  MAX31855Packet getPacket(uint32_t currMicros);
};