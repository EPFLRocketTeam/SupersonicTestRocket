/*
 * AISx120SXWrapper.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <AISx120SX.h>

// User-defined headers
#include "Packet.hpp"
#include "Sensor.hpp"
#include "macrofunctions.h"

// *************** AISx120SXPacket *************** //

/// Describe the content of a AISx120SX packet
struct AISx120SXBody
{
  float accel[2] = {0}; ///< 2 * 4 = 8 bytes
};

#define AISx120SX_BODY_FORMAT "*************** AISx120SX Packet ***************\n" \
                              "Linear accelerations:\n"                            \
                              "\t- X: %12e\n"                                      \
                              "\t- Y: %12e\n"                                      \
                              "***************** END OF PACKET ****************\n"

/**
 * @brief AISx120SX_BODY_FORMAT has 5 lines,
 *        with the opening and closing ones being 50 chars long
 *        (null char included)
 */
#define AISx120SX_BODY_PRINT_SIZE 50 * 5

/**
 * @brief Packets generated by AISx120SX sensors
 *
 */
class AISx120SXPacket : public Packet
{
public:
  // ----- Constructors ----- //

  /**
   * @brief Construct a new AISx120SXPacket based on the default header
   *
   * \c packetType_ and \c packetSize are accordingly set.
   * Packet::content is allocated.
   *
   * @see Packet::Packet()
   */
  AISx120SXPacket()
  {
    header.packetType_ = AISx120SX_PACKET_TYPE;
    header.packetSize = sizeof(AISx120SXBody);

    content = malloc(header.packetSize);
  }

  /**
   * @brief Construct a new AISx120SXPacket from a given header
   *
   * Set the header to the one provided and allocate memory according the the size
   * indicated in the header
   *
   * @param h Header for the packet, must have correct size and type
   */
  AISx120SXPacket(PacketHeader h)
  {
    assert(h.packetType_ == AISx120SX_PACKET_TYPE &&
           h.packetSize == sizeof(AISx120SXBody));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //

  /**
   * @brief Get the linear acceleration along the X axis
   *
   * @return float
   */
  float getXaccel()
  {
    return reinterpret_cast<AISx120SXBody *>(content)->accel[0];
  }

  /**
   * @brief Get the linear acceleration along the Y axis
   *
   * @return float
   */
  float getYaccel()
  {
    return reinterpret_cast<AISx120SXBody *>(content)->accel[1];
  }

  /**
   * @brief Given an array of 2 floats, fill it with the XY values of linear acceleration
   *
   * @param a Array to be filled, of size 2
   */
  void getAccel(float a[2])
  {
    a[0] = getXaccel();
    a[1] = getYaccel();
  }

  /**
   * @brief Return a pointer toward a printable description of an AISx120SX content
   *
   * @return char* : Pointer toward formated content description
   */
  char *getPrintableContent()
  {
    char output[AISx120SX_BODY_PRINT_SIZE] = "";

    snprintf(output, AISx120SX_BODY_PRINT_SIZE, AISx120SX_BODY_FORMAT,
             getXaccel(),
             getYaccel());

    return output;
  }

  // ----- Setters ----- //
  /**
   * @brief Given an array of 2 floats, set the XY values of linear acceleration in the packet
   *
   * @param a Source array
   */
  void setAccelerations(const float a[2])
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a[0];
    reinterpret_cast<AISx120SXBody *>(content)->accel[1] = a[1];
  }

  /**
   * @brief Set the linear acceleration along the X axis
   *
   * @param a_x
   */
  void setXaccel(float a_x)
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a_x;
  }

  /**
   * @brief Set the linear acceleration along the Y axis
   *
   * @param a_y
   */
  void setYaccel(float a_y)
  {
    reinterpret_cast<AISx120SXBody *>(content)->accel[0] = a_y;
  }

  /**
   * @brief Set the whole content of the packet
   *
   * @param b
   */
  void setContent(AISx120SXBody b)
  {
    memcpy(content, static_cast<void *>(&b), sizeof(AISx120SXBody));
  }
};

// *************** AISx120SXWrapper *************** //

/**
 * @brief Wrapper for AISx120SX sensors
 *
 */
class AISx120SXWrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 1250;               ///< [us] (800 Hz)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL / 2; ///< [us]
  static const uint32_t MEASUREMENT_MARGIN = 0;                    ///< [us]

  // sensor properties for error checking and conversions
  static constexpr float SENSITIVITY = 1 / (68 * 4.); ///< [g/LSB]
  static constexpr float ACC_MAX = 120;               ///< [g]
  static constexpr float ACC_MIN = -120;              ///< [g]

  AISx120SX aisObject;      ///< Underlying object
  static uint8_t sensorQty; ///< How many sensors of this type exist

  AISx120SXPacket lastPacket;

  ///< Setup parameters ; TODO: Comment them
  bandwidth bandwidthX;  ///< TODO
  bandwidth bandwidthY;  ///< TODO
  bool x_offset_monitor; ///< TODO
  bool x_offset_canc;    ///< TODO
  bool y_offset_monitor; ///< TODO
  bool y_offset_canc;    ///< TODO

public:
  // constructor
  /**
   * @brief Construct a new AISx120SXWrapper object
   *
   * Initialize AISx120SXWrapper::lastPacket
   * Also call Sensor::setupProperties with the preset values for Sensor::MEASUREMENT_INTERVAL,
   * Sensor::CHECK_INTERVAL and Sensor::MEASUREMENT_MARGIN
   * This sensor does not have a DR pin.
   *
   * @param CS_ Chip select pin assignment
   * @param bandwidthX Value for AISx120SXWrapper::bandwidthX
   * @param bandwidthY Value for AISx120SXWrapper::bandwidthY
   * @param x_offset_monitor Value for AISx120SXWrapper::x_offset_monitor
   * @param x_offset_canc Value for AISx120SXWrapper::x_offset_canc
   * @param y_offset_monitor Value for AISx120SXWrapper::y_offset_monitor
   * @param y_offset_canc Value for AISx120SXWrapper::y_offset_canc
   */
  AISx120SXWrapper(uint8_t CS_,
                   bandwidth bandwidthX, bandwidth bandwidthY,
                   bool x_offset_monitor, bool x_offset_canc,
                   bool y_offset_monitor, bool y_offset_canc);

  /// Destructor; reduce AISx120SXWrapper::sensorQty
  ~AISx120SXWrapper();

  // attemps to set up the sensor and returns true if it was successful
  /**
   * @brief  Implementation of setup for AISx120SXWrapper
   *
   * Setup is managed by the underlying object AISx120SXWrapper::adisObject
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
   * @return const char* : \c AISx120SX
   */
  const char *myName() { return "AISx120SX"; }

  /**
   * @brief Get the number of AISx120SXWrapper objects
   *
   * It should be equal to the number of AISx120SX sensors installed
   *
   * @return uint8_t : Number of AISx120SXWrapper
   */
  static uint8_t getSensorQty()
  {
    return sensorQty;
  }

  /**
   * @brief Check if the AISx120SX is due for a measurement
   *
   * A measurement is due if it is by time and one of the value is changed.
   * If there is a new value, Sensor::prevMeasTime is updated
   *
   * @param currMicros Current time, in microseconds
   * @param triggeredDR [UNUSED] Status of the DR line
   *
   * @return true : A measurement is due
   * @return false : No measurement due
   */
  bool isDue(uint32_t currMicros, unused(volatile bool &triggeredDR));

  /**
   * @brief Check if the current AISx120SXWrapper::lastPacket is invalid
   *
   * A packet is invalid if one of the values is outside its range.
   *
   * @return true : AISx120SXWrapper::lastPacket is invalid
   * @return false : AISx120SXWrapper::lastPacket is valid
   */
  bool isMeasurementInvalid();

  /**
   * @brief Generate a reference to AISx120SXWrapper::lastPacket after updating its header
   * 
   * @param currMicros Current time, in microseconds
   * @return AISx120SXPacket* : Reference to the updated AISx120SXWrapper::lastPacket
   */
  AISx120SXPacket *getPacket(uint32_t currMicros);

  /**
   * @brief Wrapper to generate AISx120SX's packet header
   *
   * Call Sensor::getHeader with correct arguments
   *
   * @param currMicros Current time, in microseconds
   * @return PacketHeader for AISx120SX
   */
  PacketHeader getHeader(uint32_t currMicros);
};