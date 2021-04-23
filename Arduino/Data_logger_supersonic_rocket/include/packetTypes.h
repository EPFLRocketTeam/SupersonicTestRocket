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
// 8 byte header for all packets
struct PacketHeader
{
  uint8_t packetType;         // 1 byte
  uint8_t packetSize;         // 1 byte
  uint8_t errorCode;          // 1 byte
  const uint8_t reserved = 0; // 1 bytes, reserved for future use
  uint32_t timestamp;         // 4 bytes
};

struct IMUPacket
{
  struct PacketHeader header; // 8 bytes
  // sending as uint16_t to save data space and since it's easy to convert
  // could likely send as float to save a potential headache later
  uint16_t gyroX;       // 2 bytes
  uint16_t gyroY;       // 2 bytes
  uint16_t gyroZ;       // 2 bytes
  uint16_t accX;        // 2 bytes
  uint16_t accY;        // 2 bytes
  uint16_t accZ;        // 2 bytes
  uint16_t temp;        // 2 bytes
  uint16_t padding = 0; // 2 bytes. necessary for alignment

  // constructor. takes data directly from wordBurst from the ADIS16470 library
  IMUPacket(uint8_t errCode, uint32_t timestamp, uint16_t IMUdata[10])
  {
    // skipping packetType 0 since that will indicate an EOF
    header.packetType = 1;
    header.packetSize = sizeof(IMUPacket); // 24 bytes
    header.errorCode = errCode;
    header.timestamp = timestamp;
    gyroX = IMUdata[1];
    gyroY = IMUdata[2];
    gyroZ = IMUdata[3];
    accX = IMUdata[4];
    accY = IMUdata[5];
    accZ = IMUdata[6];
    temp = IMUdata[7];
  }
};

struct AISx120SXPacket
{
  struct PacketHeader header; // 8 bytes
  uint16_t accelX;            // 2 bytes
  // following unused for the AIS1120SX but necessary for alignment
  uint16_t accelY; // 2 bytes

  // overloaded constructor
  AISx120SXPacket(uint8_t errCode, uint32_t timestamp,
                  uint16_t aX, uint16_t aY)
  {
    header.packetType = 2;
    header.packetSize = sizeof(AISx120SXPacket); // 12 bytes
    header.errorCode = errCode;
    header.timestamp = timestamp;
    accelX = aX;
    accelY = aY;
  }
};

struct HoneywellRSCPressurePacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float rsc015Pressure; // 4 bytes
  float rsc060Pressure; // 4 bytes

  // constructor
  HoneywellRSCPressurePacket(uint8_t errCode, uint32_t timestamp,
                             float rsc015P, float rsc060P)
  {
    header.packetType = 3;
    header.packetSize = sizeof(HoneywellRSCPressurePacket); // 16 bytes
    header.errorCode = errCode;
    header.timestamp = timestamp;
    rsc015Pressure = rsc015P;
    rsc060Pressure = rsc060P;
  }
};

struct HoneywellRSCTempPacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  float rsc015Temp; // 4 bytes
  float rsc060Temp; // 4 bytes

  // constructor
  HoneywellRSCTempPacket(uint8_t errCode, uint32_t timestamp,
                         float rsc015T, float rsc060T)
  {
    header.packetType = 4;
    header.packetSize = sizeof(HoneywellRSCTempPacket); // 16 bytes
    header.errorCode = errCode;
    header.timestamp = timestamp;
    rsc015Temp = rsc015T;
    rsc060Temp = rsc060T;
  }
};

struct ThermocouplePacket
{
  struct PacketHeader header; // 8 bytes
  // sending as floats since the computation is hard to do after acquistion
  int32_t probeTemperature[4];  // 4 * 4 = 16 bytes
  int32_t sensorTemperature[4]; // 4 * 4 = 16 bytes

  // constructor
  ThermocouplePacket(uint8_t errCode, uint32_t timestamp,
                     int32_t probeT[4], int32_t sensorT[4])
  {
    header.packetType = 5;
    header.packetSize = sizeof(ThermocouplePacket); // 40 bytes
    header.errorCode = errCode;
    header.timestamp = timestamp;
    memcpy(probeTemperature, probeT, sizeof(probeTemperature));
    memcpy(sensorTemperature, sensorT, sizeof(sensorTemperature));
  }
};