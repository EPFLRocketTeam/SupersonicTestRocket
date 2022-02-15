/*
 * AltimaxWrapper.hpp
 *
 *  Created on: 2021-11-20
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// User-defined headers
#include "Packet.hpp"
#include "Sensor.hpp"

// *************** Altimax Packets *************** //

struct AltimaxBody
{
  // Altimax has 3 pins that can output data
  // The 4th boolean is to ensure proper structure alignment (Teensy is 32-bit)
  bool pinStates[4] = {0}; // 4 * 1 = 4 bytes
};

// packet for both temperature and pressure packets
class AltimaxPacket : public Packet
{
public:
  // ----- Constructors ----- //

  AltimaxPacket()
  {
    header.packetType_ = NO_PACKET;
    header.packetSize = sizeof(AltimaxBody);

    content = malloc(header.packetSize);
  }

  AltimaxPacket(PacketHeader h)
  {
    assert(h.packetType_ == ALTIMAX_PACKET_TYPE &&
           h.packetSize == sizeof(AltimaxBody));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  float getPin0state()
  {
    return reinterpret_cast<AltimaxBody *>(content)->pinStates[0];
  }

  float getPin1state()
  {
    return reinterpret_cast<AltimaxBody *>(content)->pinStates[1];
  }

  float getPin2state()
  {
    return reinterpret_cast<AltimaxBody *>(content)->pinStates[2];
  }

  void getPinsState(bool states[3])
  {
    states[0] = getPin0state();
    states[1] = getPin1state();
    states[2] = getPin2state();
  }

  // ----- Setters ----- //
  void setPinsState(bool states[3])
  {
    reinterpret_cast<AltimaxBody *>(content)->pinStates[0] = states[0];
    reinterpret_cast<AltimaxBody *>(content)->pinStates[1] = states[1];
    reinterpret_cast<AltimaxBody *>(content)->pinStates[2] = states[2];
  }

  void setPin0state(bool state0)
  {
    reinterpret_cast<AltimaxBody *>(content)->pinStates[0] = state0;
  }

  void setPin1state(bool state1)
  {
    reinterpret_cast<AltimaxBody *>(content)->pinStates[1] = state1;
  }

  void setPin2state(bool state2)
  {
    reinterpret_cast<AltimaxBody *>(content)->pinStates[2] = state2;
  }

  void setContent(AltimaxBody b)
  {
    memcpy(content, static_cast<void *>(&b), sizeof(AltimaxBody));
  }
};

// *************** AltimaxWrapper class *************** //
class AltimaxWrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = UINT32_MAX;
  static const uint32_t CHECK_INTERVAL = UINT32_MAX;
  static const uint32_t MEASUREMENT_MARGIN = 0;

  static uint8_t sensorQty; // how many sensors of this type exist

  AltimaxPacket lastPacket;

  const uint8_t PIN_0;
  const uint8_t PIN_1;
  const uint8_t PIN_2;

public:
  // constructor
  AltimaxWrapper(uint8_t PIN_0_, uint8_t PIN_1_, uint8_t PIN_2_);

  // destructor
  ~AltimaxWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(uint32_t attempts, uint32_t delayDuration);

  const char *myName() { return "Altimax"; }

  // return the current count of sensors
  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  AltimaxPacket *getPacket(uint32_t currMicros);

  PacketHeader getHeader(uint32_t currMicros);
};