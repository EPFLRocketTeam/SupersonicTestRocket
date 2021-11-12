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
#include "Packet.hpp"
#include "Sensor.hpp"

// *************** ADIS16470Packet *************** //

struct ADIS16470Body
{
  float gyros[3] = {0};       // 3 * 4 = 12 bytes
  float acc[3] = {0};         // 3 * 4 = 12 bytes
  float temp = 0;             // 4 bytes
};

class ADIS16470Packet : public Packet
{
public:
  // ----- Constructors ----- //

  ADIS16470Packet()
  {
    header.packetType_ = ADIS16470_PACKET_TYPE;
    header.packetSize = sizeof(ADIS16470Body);

    content = malloc(header.packetSize);
  }

  ADIS16470Packet(PacketHeader h)
  {
    assert(h.packetType_ == ADIS16470_PACKET_TYPE &&
           h.packetSize == sizeof(ADIS16470Body));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  float getGyro(size_t i)
  {
    assert(i < 3);
    return reinterpret_cast<ADIS16470Body *>(content)->gyros[i];
  }

  void getGyros(float g[3])
  {
    g[0] = getGyro(0);
    g[1] = getGyro(1);
    g[2] = getGyro(2);
  }

  float getXGyro()
  {
    return getGyro(0);
  }

  float getYGyro()
  {
    return getGyro(1);
  }

  float getZGyro()
  {
    return getGyro(2);
  }

  float getAcc(size_t i)
  {
    assert(i < 3);
    return reinterpret_cast<ADIS16470Body *>(content)->acc[i];
  }

  void getAccs(float a[3])
  {
    a[0] = getAcc(0);
    a[1] = getAcc(1);
    a[2] = getAcc(2);
  }

  float getXAcc()
  {
    return getAcc(0);
  }

  float getYAcc()
  {
    return getAcc(1);
  }

  float getZAcc()
  {
    return getAcc(2);
  }

  float getTemp()
  {
    return reinterpret_cast<ADIS16470Body *>(content)->temp;
  }
  

  // ----- Setters ----- //
  void setGyro(size_t i,float g)
  {
    assert(i < 3);
    reinterpret_cast<ADIS16470Body *>(content)->gyros[i] = g;
  }

  void setGyros(const float g[3])
  {
    setGyro(0,g[0]);
    setGyro(1,g[1]);
    setGyro(2,g[2]);
  }

  void setXGyro(float g)
  {
    setGyro(0,g);
  }

  void setYGyro(float g)
  {
    setGyro(1,g);
  }

  void setZGyro(float g)
  {
    setGyro(2,g);
  }

  void setAcc(size_t i,float a)
  {
    assert(i < 3);
    reinterpret_cast<ADIS16470Body *>(content)->acc[i] = a;
  }

  void setAccs(const float a[3])
  {
    setAcc(0,a[0]);
    setAcc(1,a[1]);
    setAcc(2,a[2]);
  }

  void setXAcc(float a)
  {
    setAcc(0,a);
  }

  void setYAcc(float a)
  {
    setAcc(1,a);
  }

  void setZAcc(float a)
  {
    setAcc(2,a);
  }

  void setTemp(float t)
  {
    reinterpret_cast<ADIS16470Body *>(content)->temp = t;
  }

  void setContent(ADIS16470Body b)
  {
    memcpy(content, static_cast<void *>(&b), sizeof(ADIS16470Body));
  }
};

// *************** ADIS16470Wrapper *************** //

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
  bool setup(uint32_t attempts, uint32_t delayDuration);

  const char* myName() {return "ADIS16470";}

  uint8_t getSensorQty();

  // check if the sensor is due for a measurement
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  // check if the checksum matches
  bool verifyCheckSum(uint16_t sensorData[10]);

  // overwritten version of method in base class sensor
  bool isMeasurementInvalid();

  ADIS16470Packet *getPacket(uint32_t currMicros);

  PacketHeader getHeader(uint32_t currMicros);
};