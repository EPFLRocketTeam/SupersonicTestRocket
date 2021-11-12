/*
 * Sensor.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

#include "Packet.hpp"

// Generic sensor object
class Sensor
{
private:
  uint32_t prevCheck;             // when sensor was last checked for data
  uint32_t CHECK_INTERVAL;        // how often to check for data
  uint32_t CHECK_INTERVAL_MARGIN; // margin to start checking by time
  uint32_t MEAS_INTERVAL;         // nominal interval for data measurement

  bool DR_DRIVEN; // if the sensor has a data ready line

  bool measurementLate;  // if the last measurement was late
  int checkBeatsSkipped; // beats that were skipped since the last check
  dueType dueMethod;     // how the measurement is due
protected:
  const uint8_t SENSOR_ID; // sensor's id. non-zero if identical sensors used
  bool checksumError;      // if there was a checksum erro
  uint32_t prevMeasTime;   // when last new measurement was made
public:
  bool active; // if the sensor is active

  // constructor
  Sensor(uint8_t sensorID);

  // destructor
  ~Sensor();

  void setupProperties(uint32_t checkInterval_, uint32_t checkIntervalMargin_,
                       uint32_t measInterval_, bool DR_driven_);

  // Setup the underlying sensor (other special parameters should be given at construction)
  virtual bool setup(uint32_t attempts, uint32_t delayDuration) = 0;

  // Get the name of the sensor; useful for debugging/logging purposes
  virtual const char* myName() = 0;

  // if the sensor is due for a check because of time
  // returns 1 if it is due
  bool isDueByTime(uint32_t currMicros);

  // if the sensor is due for a read because of the DR line
  bool isDueByDR(uint32_t currMicros, volatile bool &triggeredDR);

  // if the measurement was late. should be redefined in inherited classes
  // to properly define what it means to be invalid
  bool isMeasurementLate(uint32_t currMicros);

  // if the measurement is invalid. should be redefined in inherited classes
  // to properly define what it means to be invalid
  virtual bool isMeasurementInvalid();

  // takes all the errors that happened and returns a boolean array
  bool *getErrors();

  // takes all the errors that happened and returns an error code
  uint8_t getErrorCode(uint32_t currMicros);

  // takes an error code and transforms it into a booleana array
  bool * decodeErrorCode(uint8_t errorCode);

  // generates a packet header for the sensor
  PacketHeader getHeader(packetType packetType_, uint8_t packetSize_,
                         uint32_t currMicros);

  // Auto generate Header parameters for children classes (aka specific sensors)
  virtual PacketHeader getHeader(uint32_t currMicros) = 0;

  // generate fake data when debugging the serial output without sensors
  float generateFakeData(float minValue, float maxValue,
                         uint32_t currMicros,
                         float offset = 0, uint32_t period = 5000000);
};