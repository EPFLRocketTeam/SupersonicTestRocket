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
  uint8_t packetType; // 1 byte
  uint8_t packetSize; // 1 byte
  uint8_t sensorID;   // 1 byte
  uint8_t errorCode;  // 1 byte
  uint32_t timestamp; // 4 bytes
};

struct ADIS16470Packet
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
  ADIS16470Packet(uint8_t id, uint8_t errCode, uint32_t timestamp,
                  uint16_t IMUdata[10])
  {
    // skipping packetType 0 since that will indicate an EOF
    header.packetType = 1;
    header.packetSize = sizeof(ADIS16470Packet); // 24 bytes
    header.sensorID = id;
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