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
#include "Packet.hpp"

struct MAX31855Body
{
  float probeTemperature = 0;  // 4 bytes
  float sensorTemperature = 0; // 4 bytes
};

class MAX31855Packet : public Packet
{
public:
  // ----- Constructors ----- //

  MAX31855Packet()
  {
    header.packetType_ = MAX31855_PACKET_TYPE;
    header.packetSize = sizeof(MAX31855Body);

    content = malloc(header.packetSize);
  }

  MAX31855Packet(PacketHeader h)
  {
    assert(h.packetType_ == MAX31855_PACKET_TYPE &&
           h.packetSize == sizeof(MAX31855Body));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  float getProbeTemperature()
  {
    return reinterpret_cast<MAX31855Body *>(content)->probeTemperature;
  }

  float getSensorTemperature()
  {
    return reinterpret_cast<MAX31855Body *>(content)->sensorTemperature;
  }

  // ----- Setters ----- //
  void setProbeTemperature(float t)
  {
    reinterpret_cast<MAX31855Body *>(content)->probeTemperature = t;
  }

  void setSensorTemperature(float t)
  {
    reinterpret_cast<MAX31855Body *>(content)->sensorTemperature = t;
  }

  void setContent(MAX31855Body b)
  {
    memcpy(content, static_cast<void *>(&b), sizeof(MAX31855Body));
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

  // MAX31855 header generator
  PacketHeader getHeader(uint32_t currMicros);
};