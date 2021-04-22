/*
 * logging.h
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

// SdFat
//    Available at: https://github.com/greiman/SdFat
//    Also available from the library manager.
#include <SdFat.h>
#include <RingBuf.h>
#include "globalVariables.h"

#define SD_CONFIG  SdioConfig(FIFO_SDIO)

// Size to log 64 byte lines at 2000Hz for more than sixty minutes.
#define LOG_FILE_SIZE 32*2000*60
// Space to hold 64 byte lines at 2000Hz for more than 1000ms.
#define RING_BUF_CAPACITY 32*2000*1
// Counter for the logging file name
#define COUNTER_FILENAME "counter.dat"

struct IMUPacket
{
  // skipping packetType 0 since that will indicate an EOF
  const byte packetType = 1;
  const byte packetSize = 20;
  unsigned long timestamp;
  // sending as uint16_t to save data space and since it's easy to convert
  // could likely send as float to save a potential headache later
  uint16_t gyroX;
  uint16_t gyroY;
  uint16_t gyroZ;
  uint16_t accX;
  uint16_t accY;
  uint16_t accZ;
  uint16_t temp;
};

struct AIS1120SXPacket
{
  const uint8_t packetType = 2;
  const uint8_t packetSize = 8;
  uint16_t reading;
  uint32_t timestamp;
};

struct HoneywellRSCPressurePacket
{
  const byte packetType = 3;
  const byte packetSize = 14;
  unsigned long timestamp;
  // sending as floats since the computation is hard to do after acquistion
  float rsc015Pressure;
  float rsc060Pressure;
};

struct HoneywellRSCTemperaturePacket
{
  const byte packetType = 4;
  const byte packetSize = 14;
  unsigned long timestamp;
  // could optimize and send uint16_t if there is time to do.
  // would save 4 bytes per packet
  // not critical since only running this every ~50-100ms
  float rsc015Temperature;
  float rsc060Temperature;
};

struct ThermocouplePacket
{
  const byte packetType = 5;
  const byte packetSize = 38;
  unsigned long timestamp;
  // consider change to uint16_t eventually to save 16 bytes
  // not critical since only running this every ~100ms
  int32_t probeTemperature[4];
  int32_t sensorTemperature[4];
};

void setupLoggingFile(FsFile &loggingFile,
                       RingBuf<FsFile, RING_BUF_CAPACITY> &rb);

IMUPacket encodeImuPacket(unsigned long currMicros, uint16_t imuOutput[10]);
AIS1120SXPacket encodeAIS1120SXPacket(unsigned long currMicros, uint16_t accel);
HoneywellRSCPressurePacket encodeHoneywellRSCPressurePacket(
    unsigned long currMicros, float rsc015Pressure, float rsc060Pressure);
HoneywellRSCTemperaturePacket encodeHoneywellRSCTemperaturePacket(
    unsigned long currMicros, float rsc015Temperature, float rsc060Temperature);
ThermocouplePacket encodeThermocouplePacket(unsigned long currMicros,
                                            int32_t probeTemperature[4],
                                            int32_t sensorTemperature[4]);

void binFileToCSV();