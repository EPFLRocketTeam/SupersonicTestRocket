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
#include "Sensor.h"

struct MAX31855Packet
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  int16_t probeTemperature = 0;  // 2 bytes
  int16_t sensorTemperature = 0; // 2 bytes

  // constructor for an empty packet
  MAX31855Packet(PacketHeader header_)
  {
    header = header_;
  }

  // constructor
  MAX31855Packet(PacketHeader header_, int16_t probeT, int16_t sensorT)
  {
    header = header_;
    probeTemperature = probeT;
    sensorTemperature = sensorT;
  }
};

// packet with the data decoded in floats and appropriately scaled already
struct MAX31855SerialPacket
{
  bool errors[ERROR_TYPE_NUM] = {0};
  float probeTemperature;
  float sensorTemperature;
};

// Wrapper for the MAX31855 class
class MAX31855Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 70 * 1000; // [us] (70 ms)
  static const uint32_t CHECK_INTERVAL =
      MEASUREMENT_INTERVAL / 10;                // [us] (2000Hz)
  static const uint32_t MEASUREMENT_MARGIN = 0; // [us]

  // previous measurements from the sensor
  int16_t prevProbeMeas = 0;
  int16_t prevAmbientMeas = 0;

  MAX31855_Class max31855Object;
  static uint8_t sensorQty; // how many sensors of this type exist

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

  MAX31855Packet getPacket(uint32_t currMicros);
  MAX31855SerialPacket getSerialPacket(bool debug = false);
};