#pragma once

// Standard libraries
#include <Arduino.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <SoftwareSerial.h>

// User-defined headers
#include "Packet.hpp"
#include "Sensor.hpp"
#include "macrofunctions.h"
#include "globalVariables.hpp"

#include "PacketBody/MAX7Body.h"

#define MAX7_LINE_0 "Latitude:  %8ld deg.10^-7\n"
#define MAX7_LINE_1 "Longitude: %8ld deg.10^-7\n"
#define MAX7_LINE_2 "Altitude:  %8ld mm\n"
#define MAX7_LINE_NBR 2

class MAX7Packet : public Packet
{
public:
  // ----- Constructors ----- //

  /**
   * @brief Construct a new MAX7Packet, still abstract type
   *
   * \c packetSize is accordingly set, \c packetType_ is still to be defined.
   * Packet::content is allocated
   *
   * @see Packet::Packet()
   */
  MAX7Packet()
  {
    header.packetType_ = MAX7_PACKET_TYPE;
    header.packetSize = sizeof(MAX7Body);

    content = malloc(header.packetSize);
  }

  /**
   * @brief Construct a new MAX7Packet from a given header
   *
   * Set the header to the one provided and allocate memory according the the size
   * indicated in the header
   *
   * @param h Header for the packet, must have correct size and type
   */
  MAX7Packet(PacketHeader h)
  {
    assert(h.packetType_ == MAX7_PACKET_TYPE &&
           h.packetSize == sizeof(MAX7Body));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  /**
   * @brief Get Latitude
   *
   * @return uint32_t
   */
  uint32_t getLatitude()
  {
    return reinterpret_cast<MAX7Body *>(content)->latitude;
  }

  /**
   * @brief Get Longitude
   *
   * @return uint32_t
   */
  uint32_t getLongitude()
  {
    return reinterpret_cast<MAX7Body *>(content)->longitude;
  }

  /**
   * @brief Get Altitude
   *
   * @return uint32_t
   */
  uint32_t getAltitude()
  {
    return reinterpret_cast<MAX7Body *>(content)->altitude;
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
      snprintf(buffer, DATA_SIZE, MAX7_LINE_0, getLatitude());
      return 1;

    case 1:
      snprintf(buffer, DATA_SIZE, MAX7_LINE_1, getLongitude());
      return 1;

    case 2:
      snprintf(buffer, DATA_SIZE, MAX7_LINE_2, getAltitude());
      return 0;

    default:
      return 0;
    }
  }

  /**
   * @brief Write MAX7Body in Big Endian style in \p buffer
   *
   * @warning Move \p buffer past the data
   *
   * @param buffer Buffer of size at least packetSize
   */
  void getBigEndian(void *buffer)
  {
    uint8_t *reBuffer = (uint8_t *)buffer;
    uint32_t latitude = getLatitude();
    uint32_t longitude = getLongitude();
    uint32_t altitude = getAltitude();

    BIG_ENDIAN_WRITE(latitude, reBuffer);
    BIG_ENDIAN_WRITE(longitude, reBuffer);
    BIG_ENDIAN_WRITE(altitude, reBuffer);
  }

  // ----- Setters ----- //
  /**
   * @brief Set Latitude
   *
   * @param l Provided value
   */
  void setLatitude(uint32_t l)
  {
    reinterpret_cast<MAX7Body *>(content)->latitude = l;
  }

  /**
   * @brief Set Longitude
   *
   * @param l Provided value
   */
  void setLongitude(uint32_t l)
  {
    reinterpret_cast<MAX7Body *>(content)->longitude = l;
  }

  /**
   * @brief Set Altitude
   *
   * @param l Provided value
   */
  void setAltitude(uint32_t l)
  {
    reinterpret_cast<MAX7Body *>(content)->altitude = l;
  }
};

class MAX7Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 1000 * 1000;         ///< [us] (1000 ms)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL / 10; ///< [us] (100 ms)
  static const uint32_t MEASUREMENT_MARGIN = 100;                   ///< [us], used to define how long to wait when fetching measurements

  Stream *mySerial;         ///< Serial connection
  SFE_UBLOX_GNSS gnss;      ///< Underlying object
  static uint8_t sensorQty; ///< How many sensors of this type exist
  MAX7Packet lastPacket;    ///< Holder for the packet, actualized by measurements

public:
  /**
   * @brief Construct a new MAX7Wrapper object
   *
   * Initialize MAX7Wrapper::lastPacket
   * Also call Sensor::setupProperties with the preset values for Sensor::MEASUREMENT_INTERVAL,
   * Sensor::CHECK_INTERVAL and Sensor::MEASUREMENT_MARGIN
   * This sensor does not have a DR pin.
   *
   * @param Rx Index for Rx pin
   * @param Tx Index for Tx pin
   */
  MAX7Wrapper(Stream* serial);

  /// Destructor; reduce MAX7Wrapper::sensorQty
  ~MAX7Wrapper();

  /**
   * @brief Implementation of setup for MAX7Wrapper
   *
   * A setup is successful if the underlying object returns that setup was successful
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
   * @return const char* : \c MAX7
   */
  const char *myName() { return "MAX7"; }

  /**
   * @brief Get the number of MAX7Wrapper objects
   *
   * It should be equal to the number of MAX7 sensors installed
   *
   * @return uint8_t : Number of MAX7Wrapper
   */
  uint8_t getSensorQty();

  /**
   * @brief Check if the MAX7 is due for a measurement
   *
   * A measurement is due if it is by time and the underlying object does not declare any error.
   * Update Sensor::prevMeasTime
   *
   * @param currMicros Current time, in microseconds
   * @param triggeredDR [UNUSED] Status of the DR line
   *
   * @return true : A measurement is due
   * @return false : No measurement due
   */
  bool isDue(uint32_t currMicros, unused(volatile bool &triggeredDR));

  /**
   * @brief No criteria for invalid measurement !
   *
   * @return true
   */
  bool isMeasurementInvalid();

  /**
   * @brief Generate a reference to MAX7Wrapper::lastPacket after updating its header
   *
   * @param currMicros Current time, in microseconds
   * @return MAX7Wrapper* : Reference to the updated MAX7Wrapper::lastPacket
   */
  MAX7Packet *getPacket(uint32_t currMicros);

  /**
   * @brief Wrapper to generate MAX7's packet header
   *
   * Call Sensor::getHeader with correct arguments
   *
   * @param currMicros Current time, in microseconds
   * @return PacketHeader for MAX7
   */
  PacketHeader getHeader(uint32_t currMicros);
};