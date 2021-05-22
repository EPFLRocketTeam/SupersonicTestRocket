/*
 * ADIS16470Wrapper.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <ADIS16470.h>

// User-defined headers
#include "Sensor.h"

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
  ADIS16470Packet(PacketHeader header_, uint16_t IMUdata[10])
  {
    header = header_;
    gyroX = IMUdata[1];
    gyroY = IMUdata[2];
    gyroZ = IMUdata[3];
    accX = IMUdata[4];
    accY = IMUdata[5];
    accZ = IMUdata[6];
    temp = IMUdata[7];
  }
};

// Wrapper for the ADIS16470 class
class ADIS16470Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 500;            // [us] (2000Hz)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL; // [us]
  static const uint32_t MEASUREMENT_MARGIN = 50;               // [us]

  const int DR_PIN;

  ADIS16470 adisObject;
  static uint8_t sensorQty; // how many sensors of this type exist
public:
  // constructor
  ADIS16470Wrapper(int CS, int DR, int RST);

  // destructor
  ~ADIS16470Wrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration);

  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, bool currDR);

  // check if the checksum matches
  bool verifyCheckSum(uint16_t sensorData[10]);

  ADIS16470Packet getPacket(uint32_t currMicros);
};