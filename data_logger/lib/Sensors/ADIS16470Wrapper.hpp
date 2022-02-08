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

/// Describe the content of a ADIS16470 packet
struct ADIS16470Body
{
  float gyros[3] = {0}; ///< Angular velocities around X,Y,Z axis; 3 * 4 = 12 bytes
  float acc[3] = {0};   ///< Linear velocities along X,Y,Z axis; 3 * 4 = 12 bytes
  float temp = 0;       ///< Temperature; 4 bytes
};

class ADIS16470Packet : public Packet
{
public:
  // ----- Constructors ----- //

  /**
   * @brief Construct a new ADIS16470 Packet based on the default header
   *
   * \p packetType_ and \p packetSize are accordingly set.
   * \p content is allocated.
   *
   * @see Packet::Packet()
   */
  ADIS16470Packet()
  {
    header.packetType_ = ADIS16470_PACKET_TYPE;
    header.packetSize = sizeof(ADIS16470Body);

    content = malloc(header.packetSize);
  }

  /**
   * @brief Construct a new ADIS16470 Packet from a given header
   *
   * Set the header to the one provided and allocate memory according the the size
   * indicated in the header
   *
   * @param h Header for the packet, must have correct size and type
   */
  ADIS16470Packet(PacketHeader h)
  {
    assert(h.packetType_ == ADIS16470_PACKET_TYPE &&
           h.packetSize == sizeof(ADIS16470Body));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //

  /**
   * @brief Get the wanted angular velocity 
   * 
   * @param i Index of the wanted angular velocity (0 for X, 1 for Y and 2 for Z)
   * @return float : corresponding value
   */
  float getGyro(size_t i)
  {
    assert(i < 3);
    return reinterpret_cast<ADIS16470Body *>(content)->gyros[i];
  }

  /**
   * @brief Given an array of 3 floats, fill it with the XYZ values of angular velocity
   * 
   * @param g Array to be filled
   */
  void getGyros(float g[3])
  {
    g[0] = getGyro(0);
    g[1] = getGyro(1);
    g[2] = getGyro(2);
  }

  /**
   * @brief Get the angular velocity around the X axis
   * 
   * @return float 
   * @see getGyro(size_t i)
   */
  float getXGyro()
  {
    return getGyro(0);
  }

  /**
   * @brief Get the angular velocity around the Y axis
   * 
   * @return float 
   * @see getGyro(size_t i)
   */
  float getYGyro()
  {
    return getGyro(1);
  }

  /**
   * @brief Get the angular velocity around the Z axis
   * 
   * @return float 
   * @see getGyro(size_t i)
   */
  float getZGyro()
  {
    return getGyro(2);
  }

  /**
   * @brief Get the wanted linear velocity 
   * 
   * @param i Index of the wanted linear velocity (0 for X, 1 for Y and 2 for Z)
   * @return float : corresponding value
   */
  float getAcc(size_t i)
  {
    assert(i < 3);
    return reinterpret_cast<ADIS16470Body *>(content)->acc[i];
  }

  /**
   * @brief Given an array of 3 floats, fill it with the XYZ values of linear velocity
   * 
   * @param a Array to be filled
   */
  void getAccs(float a[3])
  {
    a[0] = getAcc(0);
    a[1] = getAcc(1);
    a[2] = getAcc(2);
  }

  /**
   * @brief Get the linear velocity along the X axis
   * 
   * @return float 
   * @see getAcc(size_t i)
   */
  float getXAcc()
  {
    return getAcc(0);
  }

  /**
   * @brief Get the linear velocity along the Y axis
   * 
   * @return float 
   * @see getAcc(size_t i)
   */
  float getYAcc()
  {
    return getAcc(1);
  }

  /**
   * @brief Get the linear velocity along the Z axis
   * 
   * @return float 
   * @see getAcc(size_t i)
   */
  float getZAcc()
  {
    return getAcc(2);
  }

  /**
   * @brief Get the temperature
   * 
   * @return float 
   */
  float getTemp()
  {
    return reinterpret_cast<ADIS16470Body *>(content)->temp;
  }

  // ----- Setters ----- //

  /**
   * @brief Set the wanted angular velocity according to the provided value
   * 
   * @param i Index of the wanted angular velocity (0 for X, 1 for Y and 2 for Z)
   * @param g Provided value
   */
  void setGyro(size_t i, float g)
  {
    assert(i < 3);
    reinterpret_cast<ADIS16470Body *>(content)->gyros[i] = g;
  }

  /**
   * @brief Given an array of 3 floats, set the XYZ values of angular velocity in the packet
   * 
   * @param g Source array
   */
  void setGyros(const float g[3])
  {
    setGyro(0, g[0]);
    setGyro(1, g[1]);
    setGyro(2, g[2]);
  }

  /**
   * @brief Set the angular velocity around the X axis
   * 
   * @param g 
   */
  void setXGyro(float g)
  {
    setGyro(0, g);
  }

  /**
   * @brief Set the angular velocity around the Y axis
   * 
   * @param g 
   */
  void setYGyro(float g)
  {
    setGyro(1, g);
  }

  /**
   * @brief Set the angular velocity around the Z axis
   * 
   * @param g 
   */
  void setZGyro(float g)
  {
    setGyro(2, g);
  }

  /**
   * @brief Set the wanted linear velocity according to the provided value
   * 
   * @param i Index of the wanted linear velocity (0 for X, 1 for Y and 2 for Z)
   * @param a Provided value
   */
  void setAcc(size_t i, float a)
  {
    assert(i < 3);
    reinterpret_cast<ADIS16470Body *>(content)->acc[i] = a;
  }

  /**
   * @brief Given an array of 3 floats, set the XYZ values of linear velocity in the packet
   * 
   * @param a Source array
   */
  void setAccs(const float a[3])
  {
    setAcc(0, a[0]);
    setAcc(1, a[1]);
    setAcc(2, a[2]);
  }

  /**
   * @brief Set the linear velocity along the X axis
   * 
   * @param a 
   */
  void setXAcc(float a)
  {
    setAcc(0, a);
  }

  /**
   * @brief Set the linear velocity along the Y axis
   * 
   * @param a 
   */
  void setYAcc(float a)
  {
    setAcc(1, a);
  }

  /**
   * @brief Set the linear velocity along the Z axis
   * 
   * @param a 
   */
  void setZAcc(float a)
  {
    setAcc(2, a);
  }

  /**
   * @brief Set the temperature
   * 
   * @param t 
   */
  void setTemp(float t)
  {
    reinterpret_cast<ADIS16470Body *>(content)->temp = t;
  }

  /**
   * @brief Set the whole content of the packet
   * 
   * @param b
   */
  void setContent(ADIS16470Body &b)
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

  const char *myName() { return "ADIS16470"; }

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