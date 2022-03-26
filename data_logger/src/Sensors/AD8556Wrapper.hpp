#pragma once

// Standard libraries
#include <Arduino.h>

// External libraries
#include <AD8555.h>

// User-defined headers
#include "Packet.hpp"
#include "Sensor.hpp"
#include "macrofunctions.h"
#include "globalVariables.hpp"

#include "PacketBody/AD8556Body.h"

#define AD8556_LINE_0 "Thrust:  %6e N\n"
#define AD8556_LINE_NBR 0

class AD8556Packet : public Packet
{
public:
  // ----- Constructors ----- //

  /**
   * @brief Construct a new AD8556, still abstract type
   *
   * \c packetSize is accordingly set, \c packetType_ is still to be defined.
   * Packet::content is allocated
   *
   * @see Packet::Packet()
   */
  AD8556Packet()
  {
    header.packetType_ = AD8556_PACKET_TYPE;
    header.packetSize = sizeof(AD8556Body);

    content = malloc(header.packetSize);
  }

  /**
   * @brief Construct a new AD8556Packet from a given header
   *
   * Set the header to the one provided and allocate memory according the the size
   * indicated in the header
   *
   * @param h Header for the packet, must have correct size and type
   */
  AD8556Packet(PacketHeader h)
  {
    assert(h.packetType_ == AD8556_PACKET_TYPE &&
           h.packetSize == sizeof(AD8556Body));

    header = h;
    content = malloc(h.packetSize);
  }

  // ----- Getters ----- //
  /**
   * @brief Get reading
   *
   * @return float
   */
  float getReading()
  {
    return reinterpret_cast<AD8556Body *>(content)->reading;
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
      snprintf(buffer, DATA_SIZE, AD8556_LINE_0, getReading());
      return 0;

    default:
      return 0;
    }
  }

  /**
   * @brief Write MAX7Body in Big AD8556 style in \p buffer
   *
   * @warning Move \p buffer past the data
   *
   * @param buffer Buffer of size at least packetSize
   */
  void getBigEndian(void *buffer)
  {
    uint8_t *reBuffer = (uint8_t *)buffer;
    uint32_t reading = getReading();

    BIG_ENDIAN_WRITE(reading, reBuffer);
  }

  // ----- Setters ----- //
  /**
   * @brief Set rEADING
   *
   * @param l Provided value
   */
  void setReading(float l)
  {
    reinterpret_cast<AD8556Body *>(content)->reading = l;
  }
};

class AD8556Wrapper : public Sensor
{
private:
  static const uint32_t MEASUREMENT_INTERVAL = 10 * 1000;      ///< [us] (10 ms)
  static const uint32_t CHECK_INTERVAL = MEASUREMENT_INTERVAL; ///< [us] (1 ms)
  static const uint32_t MEASUREMENT_MARGIN = 0;                ///< [us], used to define how long to wait when fetching measurements

  AD8555 opamp;                 ///< Underlying object
  static uint8_t sensorQty;     ///< How many sensors of this type exist
  AD8556Packet lastPacket;      ///< Holder for the packet, actualized by measurements
  uint8_t Digin, Vplus, Vminus; ///< Data pins
  uint8_t firstStageGain;       ///< Opamp first stage gain
  uint8_t secondStageGain;      ///< Opamp second stage gain
  uint8_t offset;               ///< Opamp offset
  uint8_t analogResolution;     ///< Analog read resolution on the microcontroller
  float minReading;             ///< Minimum reading expected from sensor connected to AD8556
  float maxReading;             ///< Maximum reading expected from sensor connected to AD8556
  float rescale;                ///< Linear value used when rescaling readings; exactly (maxReading-minReading) / (2 << analogResolution)

public:
  /**
   * @brief Construct a new AD8556Wrapper object
   *
   * Initialize AD8556Wrapper::lastPacket
   * Also call Sensor::setupProperties with the preset values for Sensor::MEASUREMENT_INTERVAL,
   * Sensor::CHECK_INTERVAL and Sensor::MEASUREMENT_MARGIN
   * This sensor does not have a DR pin.
   *
   * @param digin Index for Digin pin
   * @param vout Index for Vout pin
   * @param firstStageGain_ Code for the gain for the first stage (0..127)
   * @param secondStageGain_ Code for the gain for second stage (0..7)
   * @param offset_ Code for the offset (0..255)
   * @param analogResolution_ Analog read resolution on the microcontroller
   * @param minRead Minimum reading expected from sensor connected to AD8556
   * @param maxRead Maximum reading expected from sensor connected to AD8556
   */
  AD8556Wrapper(uint8_t digin, uint8_t vplus, uint8_t vminus, uint8_t firstStageGain_,
                uint8_t secondStageGain_, uint8_t offset_,
                uint8_t analogResolution_, float minRead, float maxRead);

  /// Destructor; reduce AD8556Wrapper::sensorQty
  ~AD8556Wrapper();

  /**
   * @brief Implementation of setup for AD8556Wrapper
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
   * @return const char* : \c AD8556
   */
  const char *myName() { return "AD8556"; }

  /**
   * @brief Get the number of AD8556Wrapper objects
   *
   * It should be equal to the number of AD8556 sensors installed
   *
   * @return uint8_t : Number of AD8556Wrapper
   */
  uint8_t getSensorQty();

  /**
   * @brief Check if the AD8556 is due for a measurement
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
   * @brief Generate a reference to AD8556Wrapper::lastPacket after updating its header
   *
   * @param currMicros Current time, in microseconds
   * @return AD8556Wrapper* : Reference to the updated AD8556Wrapper::lastPacket
   */
  AD8556Packet *getPacket(uint32_t currMicros);

  /**
   * @brief Wrapper to generate AD8556's packet header
   *
   * Call Sensor::getHeader with correct arguments
   *
   * @param currMicros Current time, in microseconds
   * @return PacketHeader for AD8556
   */
  PacketHeader getHeader(uint32_t currMicros);
};