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
#include "Packet.hpp"
#include "Sensor.hpp"

// *************** AISx120SXPacket *************** //

struct AISx120SXBody
{
  float accel[2] = {0}; // 2 * 4 = 8 bytes
};

class AISx120SXPacket : public Packet
{
public:
  // ----- Constructors ----- //

  AISx120SXPacket()
  {
    header.packetType_ = AISx120SX_PACKET_TYPE;
    header.packetSize = sizeof(AISx120SXBody);

    content = malloc(header.packetSize);
  }

  AISx120SXPacket(PacketHeader h)
  {
    assert(h.packetType_ == AISx120SX_PACKET_TYPE &&
           h.packetSize == sizeof(AISx120SXBody));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  float getXaccel()
  {
    return reinterpret_cast<AISx120SXBody *>(content)->accel[0];
  }

  float getYaccel()
  {
    return reinterpret_cast<AISx120SXBody *>(content)->accel[1];
  }

  void getAccel(float a[2])
  {
    a[0] = getXaccel();
    a[1] = getYaccel();
  }

  // ----- Setters ----- //
  void setAcceleration(const float a[2])
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a[0];
    reinterpret_cast<AISx120SXBody *>(content)->accel[1] = a[1];
  }

  void setXaccel(float a_x)
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a_x;
  }

  void setYaccel(float a_y)
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a_y;
  }

  void setContent(AISx120SXBody b)
  {
    memcpy(content, static_cast<void *>(&b), sizeof(AISx120SXBody));
  }
};

// *************** AISx120SXWrapper *************** //

class AISx120SXWrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 1250; // [us] (800 Hz)
  static const uint32_t CHECK_INTERVAL =
      MEASUREMENT_INTERVAL / 2;                 // [us]
  static const uint32_t MEASUREMENT_MARGIN = 0; // [us]

  // sensor properties for error checking and conversions
  static constexpr float SENSITIVITY = 1 / (68 * 4.); // [g/LSB]
  static constexpr float ACC_MAX = 120;
  static constexpr float ACC_MIN = -120;

  AISx120SX aisObject;
  static uint8_t sensorQty; // how many sensors of this type exist

  AISx120SXPacket lastPacket;

  // Setup parameters
  bandwidth bandwidthX;
  bandwidth bandwidthY;
  bool x_offset_monitor;
  bool x_offset_canc;
  bool y_offset_monitor;
  bool y_offset_canc;

public:
  // constructor
  AISx120SXWrapper(uint8_t CS_,
                   bandwidth bandwidthX, bandwidth bandwidthY,
                   bool x_offset_monitor, bool x_offset_canc,
                   bool y_offset_monitor, bool y_offset_canc);

  // destructor
  ~AISx120SXWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(uint32_t attempts, uint32_t delayDuration);

  const char *myName() { return "AISx120SX"; }

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  AISx120SXPacket *getPacket(uint32_t currMicros);

  PacketHeader getHeader(uint32_t currMicros);
};