/*
 * packetTypes.h
 *
 *  Created on: 2021-04-23
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Packet definitions. If packets are discontinued in the future, they
// SHOULD NOT BE REMOVED FROM HERE! Instead, they should be commented to allow
// easy decoding of older packet types.

struct AISx120SXPacket
{
  struct PacketHeader header; // 8 bytes
  uint16_t accelX;            // 2 bytes
  // following unused for the AIS1120SX but necessary for alignment
  uint16_t accelY; // 2 bytes

  // overloaded constructor
  AISx120SXPacket(uint8_t id, uint8_t errCode, uint32_t timestamp,
                  int16_t acceleration[2])
  {
    header.packetType = 2;
    header.packetSize = sizeof(AISx120SXPacket); // 12 bytes
    header.sensorID = id;
    header.errorCode = errCode;
    header.timestamp = timestamp;
    accelX = acceleration[0];
    accelY = acceleration[1];
  }
};

struct HoneywellRSCPressurePacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float pressure; // 4 bytes

  // constructor
  HoneywellRSCPressurePacket(uint8_t id, uint8_t errCode, uint32_t timestamp,
                             float p)
  {
    header.packetType = 3;
    header.packetSize = sizeof(HoneywellRSCPressurePacket); // 12 bytes
    header.sensorID = id;
    header.errorCode = errCode;
    header.timestamp = timestamp;
    pressure = p;
  }
};

struct HoneywellRSCTempPacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float temperature; // 4 bytes

  // constructor
  HoneywellRSCTempPacket(uint8_t id, uint8_t errCode, uint32_t timestamp,
                         float T)
  {
    header.packetType = 4;
    header.packetSize = sizeof(HoneywellRSCTempPacket); // 12 bytes
    header.sensorID = id;
    header.errorCode = errCode;
    header.timestamp = timestamp;
    temperature = T;
  }
};

struct ThermocouplePacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  int16_t probeTemperature;  // 2 bytes
  int16_t sensorTemperature; // 2 bytes

  // constructor
  ThermocouplePacket(uint8_t id, uint8_t errCode, uint32_t timestamp,
                     int16_t probeT, int16_t sensorT)
  {
    header.packetType = 5;
    header.packetSize = sizeof(ThermocouplePacket); // 24 bytes
    header.sensorID = id;
    header.errorCode = errCode;
    header.timestamp = timestamp;
    probeTemperature = probeT;
    sensorTemperature = sensorT;
  }
};