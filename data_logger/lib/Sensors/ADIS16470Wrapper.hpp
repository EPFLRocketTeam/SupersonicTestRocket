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
#include "Sensor.hpp"

struct ADIS16470Packet
{
  struct PacketHeader header; // 8 bytes
  float gyros[3] = {0};       // 3 * 4 = 12 bytes
  float acc[3] = {0};         // 3 * 4 = 12 bytes
  float temp = 0;             // 4 bytes

  // constructor for an empty packet
  ADIS16470Packet(PacketHeader header_)
  {
    header = header_;
  }
};

// Wrapper for the ADIS16470 class
class ADIS16470Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 500;            // [us] (2000Hz)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL; // [us]
  static const uint32_t MEASUREMENT_MARGIN = 500;              // [us]

  // sensor properties for error checking and conversions
  static constexpr float GYRO_SENSITIVITY = 0.1; // [deg/s /LSB]
  static constexpr float GYRO_MAX = 2000;        // [deg/s]
  static constexpr float GYRO_MIN = -2000;
  static constexpr float ACC_SENSITIVITY = 0.00125; // [g/LSB]
  static constexpr float ACC_MAX = 40;              // [g]
  static constexpr float ACC_MIN = -40;
  static constexpr float TEMP_SENSITIVITY = 0.1; // [degC/LSB]
  static constexpr float TEMP_MAX = 85;          // [degC]
  static constexpr float TEMP_MIN = -25;

  const int DR_PIN;

  ADIS16470 adisObject;
  static uint8_t sensorQty; // how many sensors of this type exist

  ADIS16470Packet lastPacket;

public:
  // constructor
  ADIS16470Wrapper(int CS, int DR, int RST);

  // destructor
  ~ADIS16470Wrapper();

  // attemps to set up the sensor and returns true if it was successful
  bool setup(int attempts, int delayDuration);

  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // check if the checksum matches
  bool verifyCheckSum(uint16_t sensorData[10]);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  ADIS16470Packet getPacket(uint32_t currMicros);
};