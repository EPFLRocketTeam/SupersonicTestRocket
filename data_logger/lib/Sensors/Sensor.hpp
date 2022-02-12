/*
 * Sensor.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

#include "Packet.hpp"

/**
 * @brief Generic sensor object.
 *
 * Used to regroup the Wrappers for the different 'actual' sensors
 */
class Sensor
{
private:
  uint32_t prevCheck;             ///< When the sensor was last checked for data (in microseconds)
  uint32_t CHECK_INTERVAL;        ///< How often to check for data (in microseconds)
  uint32_t CHECK_INTERVAL_MARGIN; ///< Margin to start checking by time (in microseconds)
  uint32_t MEAS_INTERVAL;         ///< Nominal interval for data measurement (in microseconds)

  bool DR_DRIVEN; ///< If the sensor has a data ready line

  bool measurementLate;  ///< If the last measurement was late
  int checkBeatsSkipped; ///< Number of beats skipped since the last check
  dueType dueMethod;     ///< How the measurement is due (by time )
protected:
  const uint8_t SENSOR_ID; ///< Sensor's id. Is non-zero if identical sensors are used
  bool checksumError;      ///< If there was a checksum erro
  uint32_t prevMeasTime;   ///< When last new measurement was made (in microseconds)

  /**
   * @brief Get the array of possible errors for a sensor
   *
   * First error: if a measurement beat was missed (measurement beat skipped)
   * Second error: if the acquisition loop skipped a beat
   * Third error: if DR pin didn't trigger the read
   * Fourth error: checksum error
   * Fifth error: invalid measurement
   *
   * \note Ensure the number of errors matches ERROR_TYPE_NUM
   *
   * @see ERROR_TYPE_NUM
   */
  bool errors[ERROR_TYPE_NUM] = {false};

public:
  bool active; ///< If the sensor is active

  /**
   * @brief Construct a new Sensor object. Only set Sensor::SENSOR_ID
   *
   * @param sensorID
   *
   * @see Sensor::SENSOR_ID
   */
  Sensor(uint8_t sensorID);

  /**
   * @brief Destroy the Sensor object (default destructor)
   */
  ~Sensor();

  /**
   * @brief Set the Sensor's measure attributes according to the provided arguments
   *
   * @param checkInterval_ Value for Sensor::CHECK_INTERVAL
   * @param checkIntervalMargin_ Value for Sensor::CHECK_INTERVAL_MARGIN
   * @param measInterval_ Value for Sensor::MEAS_INTERVAL
   * @param DR_driven_ Value for Sensor::DR_DRIVEN
   */
  void setupProperties(uint32_t checkInterval_, uint32_t checkIntervalMargin_,
                       uint32_t measInterval_, bool DR_driven_);

  /**
   * @brief Setup the underlying sensor (other parameters should be given at construction)
   *
   * @param attempts Number of allowed attempts to try setting up the sensor
   * @param delayDuration Delay between the tries
   * @return true : The Sensor is correctly set up
   * @return false : The Sensor failed to set up
   */
  virtual bool setup(uint32_t attempts, uint32_t delayDuration) = 0;

  /**
   * @brief Get the name of the sensor; mostly for logging/debugging purpose
   *
   * @return const char*
   */
  virtual const char *myName() = 0;

  /**
   * @brief Returns if the sensor if due for a read because of time elapsed, and actualize corresponding attributes
   *
   * First, compute the new value of Sensor::checkBeatsSkipped
   * \f[
   * \texttt{checkBeatsSkipped} = \left\lfloor \cfrac{\texttt{currMicros} - \texttt{prevCheck}}{\texttt{CHECK_INTERVAL}} \right\rfloor
   * \f]
   *
   * If the sensor was already reading by time (that is Sensor::dueMethod is dueType::DUE_BY_TIME ),
   * we remove the margin Sensor::CHECK_INTERVAL_MARGIN and simply rely on the nominal checking interval Sensor::CHECK_INTERVAL .
   * If the time elapsed between the current time \p currMicros and the time of the last check Sensor::prevCheck is larger than the interval,
   * we set the Sensor::dueMethod to dueType::DUE_BY_TIME , set Sensor::prevCheck to \p currMicros and return \p true .
   *
   * \note Sensor::dueMethod can be switched to  dueType::DUE_BY_DR only by Sensor::isDueByDR
   *
   * @param currMicros Current time, in microseconds
   * @return true : A measurement is due because of the time elapsed since the last collected one
   * @return false : No measurement due by time
   */
  bool isDueByTime(uint32_t currMicros);

  /**
   * @brief Returns if the sensor is due for a read because of the Data Ready line
   *
   * If the sensor has the Sensor::DR_DRIVEN flag set and Sensor::triggeredDR is true,
   * \p triggeredDR is reset, Sensor::dueMethod is actualized to dueType::DUE_BY_DR and Sensor::prevCheck is set to \p currMicros .
   *
   *
   * @param currMicros Current time, in microseconds
   * @param triggeredDR Status of the DR line
   * @return true :  A measurement is due because of the DR line
   * @return false : No measurement due by the DR line
   *
   * @see Sensor::isDueByTime(uint32_t currMicros)
   */
  bool isDueByDR(uint32_t currMicros, volatile bool &triggeredDR);

  /**
   * @brief Add a virtual, general version to streamline management.
   *        Should be implemented by child classes
   *
   * @param currMicros Current time, in microseconds
   * @param triggeredDR Status of the DR line. Might be dropped by child classes not using it
   *
   * @return true : A measurement is due
   * @return false : No measurement due
   */
  virtual bool isDue(uint32_t currMicros, volatile bool &triggeredDR) = 0;

  /**
   * @brief Update the value of Sensor::measurementLate and returns it
   * 
   * A measurement is considered late is the time elapsed since the last one ( Sensor::prevMeasTime )
   * is at least twice the measurement interval ( Sensor::MEAS_INTERVAL )
   * 
   * @param currMicros Current time, in microseconds
   * @return true : A measurement should be done
   * @return false : No measurement to be done
   */
  bool isMeasurementLate(uint32_t currMicros);

  /**
   * @brief Implemented by child classes. Used to declare a measurement invalid
   */
  virtual bool isMeasurementInvalid() = 0;

  /**
   * @brief Update the \p errors bool array
   * 
   * @param currMicros Current time, in microseconds
   * @see Sensor::errors
   */
  void updateErrors(uint32_t currMicros);

  /**
   * @brief Generate a \c PacketHeader from the sensor and the explicitely given arguments
   * 
   * @note It is preferred to use its virtual counterpart, as both \p packetType_ and \p packetSize_
   * should be automatically well-chosen.
   * 
   * @param packetType_ Type of the packet
   * @param packetSize_ Size of the packet, in bytes
   * @param currMicros  Current time, in microseconds
   * @return PacketHeader 
   * 
   * @see PacketHeader
   * @see packetType
   * @see Sensor::getHeader(uint32_t currMicros)
   */
  PacketHeader getHeader(packetType packetType_, uint8_t packetSize_,
                         uint32_t currMicros);

  /**
   * @brief Auto generate the PacketHeader object; defined in child classes
   *
   * @param currMicros : Current time, in microseconds
   * @return PacketHeader
   *
   * @see PacketHeader
   */
  virtual PacketHeader getHeader(uint32_t currMicros) = 0;

  /**
   * @brief Get a reference to the last packet provided by the sensor
   * 
   * This function may be in charge of actualizing the content of the packet buffer.
   * 
   * @param currMicros Current time, in microseconds
   * @return Packet* : Reference to the current packet held
   * 
   * @see Packet
   */
  virtual Packet *getPacket(uint32_t currMicros) = 0;
};

/**
 * @brief Generate some fake data; useful for debugging
 *
 * Compute \f$ \texttt{mid} = \cfrac{\texttt{minValue} + \texttt{maxValue}}{2} \f$
 * and \f$ \texttt{amplitude} = \cfrac{\texttt{maxValue} - \texttt{minValue}}{2} \f$
 *
 * Pseudo-random output: \f$ \sin(2\pi * \texttt{currMicros} / \texttt{period}) * \texttt{amplitude} + \texttt{mid} + \texttt{offset} \f$
 *
 * @param minValue : Minimal possible value
 * @param maxValue : Maximal possible value
 * @param currMicros : Current time, in microseconds
 * @param offset : Offset from the mid value
 * @param period : Temporal period of the sin function used to compute output (in microseconds)
 * @return float : A pseudo-random number
 */
float generateFakeData(float minValue, float maxValue,
                       uint32_t currMicros,
                       float offset = 0, uint32_t period = 5000000);

/**
 * @brief Convert a boolean errors' array to an error code (binary encoding)
 * 
 * @param errorArray An error code array, of size ERROR_TYPE_NUM
 * @return uint8_t : The corresponding error code
 * 
 * @see Sensor::errors
 */
uint8_t getErrorCode(bool *errorArray);

/**
 * @brief Convert an error code into the corresponding boolean array
 * 
 * @param errorArray An error to store the result
 * @param errorCode An error code (errors encoded in binary)
 * 
 * @see Sensor::errors
 */
void decodeErrorCode(bool errorArray[ERROR_TYPE_NUM], uint8_t errorCode);