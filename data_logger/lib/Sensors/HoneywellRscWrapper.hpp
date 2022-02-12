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
#include "Packet.hpp"
#include "Sensor.hpp"

// *************** HoneywellRsc Packets *************** //

struct HoneywellRSCBody
{
  // sending as floats since the computation is hard to do after acquistion
  float measurement = 0; // 4 bytes
};

// packet for both temperature and pressure packets
class HoneywellRSCPacket : public Packet
{
public:
  // ----- Constructors ----- //

  HoneywellRSCPacket()
  {
    header.packetType_ = NO_PACKET;
    header.packetSize = sizeof(HoneywellRSCBody);

    content = malloc(header.packetSize);
  }

  // ----- Getters ----- //

  float getMeasurement()
  {
    return reinterpret_cast<HoneywellRSCBody *>(content)->measurement;
  }

  // ----- Setters ----- //

  void setMeasurement(float m)
  {
    reinterpret_cast<HoneywellRSCBody *>(content)->measurement = m;
  }
};

class HoneywellRSC_Temp_Packet : public HoneywellRSCPacket
{
public:
  // ----- Constructors ----- //
  HoneywellRSC_Temp_Packet()
  {
    header.packetType_ = RSC_TEMP_PACKET_TYPE;
    header.packetSize = sizeof(HoneywellRSCBody);

    content = malloc(header.packetSize);
  }

  HoneywellRSC_Temp_Packet(PacketHeader h)
  {
    assert(h.packetType_ == RSC_TEMP_PACKET_TYPE &&
           h.packetSize == sizeof(HoneywellRSCBody));

    header = h;
    content = malloc(h.packetSize);
  }
};

class HoneywellRSC_Pressure_Packet : public HoneywellRSCPacket
{
public:
  // ----- Constructors ----- //
  HoneywellRSC_Pressure_Packet()
  {
    header.packetType_ = RSC_PRESSURE_PACKET_TYPE;
    header.packetSize = sizeof(HoneywellRSCBody);

    content = malloc(header.packetSize);
  }

  HoneywellRSC_Pressure_Packet(PacketHeader h)
  {
    assert(h.packetType_ == RSC_PRESSURE_PACKET_TYPE &&
           h.packetSize == sizeof(HoneywellRSCBody));

    header = h;
    content = malloc(h.packetSize);
  }
};

// *************** HoneywellRscWrapper class *************** //
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

  HoneywellRSC_Pressure_Packet lastPressurePacket;
  HoneywellRSC_Temp_Packet lastTempPacket;

  // Parameters for setup
  RSC_DATA_RATE data_rate;
  uint32_t desiredTempPeriod;

public:
  // constructor
  HoneywellRscWrapper(int DR, int CS_EE, int CS_ADC, int SPI_BUS,
                      RSC_DATA_RATE data_r, uint32_t desired_P);

  // destructor
  ~HoneywellRscWrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(uint32_t attempts, uint32_t delayDuration);

  const char *myName() { return "HoneywellRsc"; }

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

  HoneywellRSCPacket *getPacket(uint32_t currMicros);
  HoneywellRSCPacket *getSerialPackets(uint32_t currMicros);

  PacketHeader getHeader(uint32_t currMicros);
};