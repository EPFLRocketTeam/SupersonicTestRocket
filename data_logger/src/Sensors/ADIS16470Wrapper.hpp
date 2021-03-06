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
#include "globalVariables.hpp"
#include "macrofunctions.h"

#include "PacketBody/ADIS16470Body.h"

// *************** ADIS16470Packet *************** //

#define ADIS16470_LINE_0 "Angular velocities:\n"
#define ADIS16470_LINE_1 "\t- X: %6e °/s\n"
#define ADIS16470_LINE_2 "\t- Y: %6e °/s\n"
#define ADIS16470_LINE_3 "\t- Z: %6e °/s\n"
#define ADIS16470_LINE_4 "Linear acceleration:\n"
#define ADIS16470_LINE_5 "\t- X: %6e g\n"
#define ADIS16470_LINE_6 "\t- Y: %6e g\n"
#define ADIS16470_LINE_7 "\t- Z: %6e g\n"
#define ADIS16470_LINE_8 "Temperature: %6e °C\n"
#define ADIS16470_LINE_NBR 8

/**
 * @brief Packets generated by ADIS16470 sensors
 *
 */
class ADIS16470Packet : public Packet
{
public:
  // ----- Constructors ----- //

  /**
   * @brief Construct a new ADIS16470Packet based on the default header
   *
   * \c packetType_ and \c packetSize are accordingly set.
   * Packet::content is allocated.
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
   * @brief Construct a new ADIS16470Packet from a given header
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
   * @param g Array to be filled, of size 3
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
   * @brief Get the wanted linear acceleration
   *
   * @param i Index of the wanted linear acceleration (0 for X, 1 for Y and 2 for Z)
   * @return float : corresponding value
   */
  float getAcc(size_t i)
  {
    assert(i < 3);
    return reinterpret_cast<ADIS16470Body *>(content)->acc[i];
  }

  /**
   * @brief Given an array of 3 floats, fill it with the XYZ values of linear acceleration
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
   * @brief Get the linear acceleration along the X axis
   *
   * @return float
   * @see getAcc(size_t i)
   */
  float getXAcc()
  {
    return getAcc(0);
  }

  /**
   * @brief Get the linear acceleration along the Y axis
   *
   * @return float
   * @see getAcc(size_t i)
   */
  float getYAcc()
  {
    return getAcc(1);
  }

  /**
   * @brief Get the linear acceleration along the Z axis
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

  /**
   * @brief Fill \p buffer with the \p lineNbr line of content to be displayed
   *
   * If the line queried does not exist ( \p lineNbr too big), does nothing to \p buffer
   *
   * @param buffer Buffer to be filled (with up to DATA_SIZE chars)
   * @param lineNbr Number of the queried line
   * @return int : 1 if there are lines after \p lineNbr , 0 otherwise
   */
  int getPrintableContent(char *buffer, size_t lineNbr)
  {
    switch (lineNbr)
    {
    case 0:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_0);
      return 1;

    case 1:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_1, getXGyro());
      return 1;

    case 2:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_2, getYGyro());
      return 1;

    case 3:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_3, getZGyro());
      return 1;

    case 4:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_4);
      return 1;

    case 5:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_5, getXAcc());
      return 1;

    case 6:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_6, getYAcc());
      return 1;

    case 7:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_7, getZAcc());
      return 1;

    case 8:
      snprintf(buffer, DATA_SIZE, ADIS16470_LINE_8, getTemp());
      return 0;

    default:
      return 0;
    }
  }

  /**
   * @brief Write ADIS16470Body in Big Endian style in \p buffer
   *
   * @warning Move \p buffer past the data
   *
   * @param buffer Buffer of size at least packetSize
   */
  void getBigEndian(void *buffer)
  {
    // Re cast the buffer to write byte per byte
    uint8_t *reBuffer = (uint8_t *)buffer;
    float gyroX, gyroY, gyroZ;
    float accX, accY, accZ;
    float temp;

    gyroX = getXGyro();
    gyroY = getYGyro();
    gyroZ = getZGyro();
    accX = getXAcc();
    accY = getYAcc();
    accZ = getZAcc();
    temp = getTemp();
    
    BIG_ENDIAN_WRITE(gyroX, reBuffer);
    BIG_ENDIAN_WRITE(gyroY, reBuffer);
    BIG_ENDIAN_WRITE(gyroZ, reBuffer);

    BIG_ENDIAN_WRITE(accX, reBuffer);
    BIG_ENDIAN_WRITE(accY, reBuffer);
    BIG_ENDIAN_WRITE(accZ, reBuffer);

    BIG_ENDIAN_WRITE(temp, reBuffer);
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
   * @see setGyros(const float g[3])
   */
  void setXGyro(float g)
  {
    setGyro(0, g);
  }

  /**
   * @brief Set the angular velocity around the Y axis
   *
   * @param g
   * @see setGyros(const float g[3])
   */
  void setYGyro(float g)
  {
    setGyro(1, g);
  }

  /**
   * @brief Set the angular velocity around the Z axis
   *
   * @param g
   * @see setGyros(const float g[3])
   */
  void setZGyro(float g)
  {
    setGyro(2, g);
  }

  /**
   * @brief Set the wanted linear acceleration according to the provided value
   *
   * @param i Index of the wanted linear acceleration (0 for X, 1 for Y and 2 for Z)
   * @param a Provided value
   */
  void setAcc(size_t i, float a)
  {
    assert(i < 3);
    reinterpret_cast<ADIS16470Body *>(content)->acc[i] = a;
  }

  /**
   * @brief Given an array of 3 floats, set the XYZ values of linear acceleration in the packet
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
   * @brief Set the linear acceleration along the X axis
   *
   * @param a
   * @see setAcc(size_t i, float a)
   */
  void setXAcc(float a)
  {
    setAcc(0, a);
  }

  /**
   * @brief Set the linear acceleration along the Y axis
   *
   * @param a
   * @see setAcc(size_t i, float a)
   */
  void setYAcc(float a)
  {
    setAcc(1, a);
  }

  /**
   * @brief Set the linear acceleration along the Z axis
   *
   * @param a
   * @see setAcc(size_t i, float a)
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

/**
 * @brief Wrapper for ADIS16470 sensors
 */
class ADIS16470Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 500;            ///< [us] (2000Hz)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL; ///< [us]
  static const uint32_t MEASUREMENT_MARGIN = 500;              ///< [us]

  // sensor properties for error checking and conversions
  static constexpr float GYRO_SENSITIVITY = 0.1;    ///< [deg/s /LSB]
  static constexpr float GYRO_MAX = 2000;           ///< [deg/s]
  static constexpr float GYRO_MIN = -2000;          ///< [deg/s]
  static constexpr float ACC_SENSITIVITY = 0.00125; ///< [g/LSB]
  static constexpr float ACC_MAX = 40;              ///< [g]
  static constexpr float ACC_MIN = -40;             ///< [g]
  static constexpr float TEMP_SENSITIVITY = 0.1;    ///< [degC/LSB]
  static constexpr float TEMP_MAX = 85;             ///< [degC]
  static constexpr float TEMP_MIN = -25;            ///< [degC]

  // const int DR_PIN; ///< UNUSED!

  ADIS16470 adisObject;                     ///< Underlying object
  static const size_t wordBurstLength = 10; ///< Hardcode here the length of the array returned by adisObject::wordBurst()
  static const size_t byteBurstLength = 20; ///< Hardcode here the length of the array returned by adisObject::byteBurst()

  static uint8_t sensorQty; ///< How many sensors of this type exist

  ADIS16470Packet lastPacket; ///< Holder for the packet, actualized by measurements

public:
  /**
   * @brief Construct a new ADIS16470Wrapper object
   *
   * Initialize ADIS16470Wrapper::lastPacket
   * Also call Sensor::setupProperties with the preset values for Sensor::MEASUREMENT_INTERVAL,
   * Sensor::CHECK_INTERVAL and Sensor::MEASUREMENT_MARGIN
   * This sensor has a DR pin.
   *
   * @param CS Index for CS pin
   * @param DR Index for DR pin
   * @param RST Index for RST pin
   */
  ADIS16470Wrapper(int CS, int DR, int RST);

  /// Destructor; reduce ADIS16470Wrapper::sensorQty
  ~ADIS16470Wrapper();

  /**
   * @brief Implementation of setup for ADIS16470Wrapper
   *
   * A setup is successful if we can have a non-zeros reading with correct checksum
   *
   * @param attempts Number of allowed attempts to try setting up the sensor
   * @param delayDuration Delay between the tries
   * @return true : The Sensor is correctly set up
   * @return false : The Sensor failed to set up
   */
  bool setup(uint32_t attempts, uint32_t delayDuration);

  /**
   * @brief Get the name of this sensor as a string
   *
   * @return const char* : \c ADIS16470
   */
  const char *myName() { return "ADIS16470"; }

  /**
   * @brief Get the number of ADIS16470Wrapper objects
   *
   * It should be equal to the number of ADIS16470 sensors installed
   *
   * @return uint8_t : Number of ADIS16470Wrapper
   */
  static uint8_t getSensorQty()
  {
    return sensorQty;
  }

  /**
   * @brief Check if the ADIS16470 is due for a measurement
   *
   * A measurement is due if it is by time or by DR line. Update Sensor::prevMeasTime
   *
   * @param currMicros Current time, in microseconds
   * @param triggeredDR Status of the DR line
   *
   * @return true : A measurement is due
   * @return false : No measurement due
   */
  bool isDue(uint32_t currMicros, volatile bool &triggeredDR);

  /**
   * @brief Verify if the checksum is right
   *
   * The verification is done by the underlying ADIS16470Wrapper::adisObject
   *
   * @param sensorData Data to have its checksum verified
   * @return true : Checksum is correct
   * @return false : Checksum is incorrect
   */
  bool verifyCheckSum(uint16_t sensorData[10]);

  /**
   * @brief Check if the current ADIS16470Wrapper::lastPacket is invalid
   *
   * A packet is invalid if it is either only zeros or one of the values is outside
   * its range.
   *
   * @return true : ADIS16470Wrapper::lastPacket is invalid
   * @return false : ADIS16470Wrapper::lastPacket is valid
   */
  bool isMeasurementInvalid();

  /**
   * @brief Generate a reference to ADIS16470Wrapper::lastPacket after updating it
   *
   * @param currMicros Current time, in microseconds
   * @return ADIS16470Packet* : Reference to the updated ADIS16470Wrapper::lastPacket
   */
  ADIS16470Packet *getPacket();

  /**
   * @brief Wrapper to generate ADIS16470's packet header
   *
   * Call Sensor::getHeader with correct arguments
   *
   * @param currMicros Current time, in microseconds
   * @return PacketHeader for ADIS16470
   */
  PacketHeader getHeader(uint32_t currMicros);
};