/*
 * Sensor.h
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

// define how measurements can be due
typedef enum
{
  NOT_DUE,     // measurement not due
  DUE_BY_TIME, // due by time
  DUE_BY_DR    // due by DR input
} dueType;

// define packet type numbering
enum
{
  NO_PACKET,                // no packet. indicated EOF
  ADIS16470_PACKET_TYPE,    // ADIS16470 Packet
  AISx120SX_PACKET_TYPE,    // AISx120SX Packet
  RSC_PRESSURE_PACKET_TYPE, // Honewell RSC pressure packet
  RSC_TEMP_PACKET_TYPE,     // Honewell RSC temperature packet
  MAX31855_PACKET_TYPE,     // MAX31855 packet
  ALTIMAX_PACKET_TYPE       // ALTIMAX packet
};
typedef uint8_t packetType;

// 8 byte header for all packets
struct PacketHeader
{
  packetType packetType_; // 1 byte
  uint8_t packetSize;     // 1 byte
  uint8_t sensorID;       // 1 byte
  uint8_t errorCode;      // 1 byte
  uint32_t timestamp;     // 4 bytes
};

// simpler, generic packet for serial output
struct serialPacket
{
  bool errors[5];
  // TODO: serialPacket has a defined size since I'm lazy to do it with pointers
  // this is not very memory efficient...
  float readings[7];
};

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

  // if the sensor is due for a check because of time
  // returns 1 if it is due
  bool isDueByTime(uint32_t currMicros);

  // if the sensor is due for a read because of the DR line
  bool isDueByDR(uint32_t currMicros, volatile bool &triggeredDR);

  // if the measurement was late
  bool isMeasurementLate(uint32_t currMicros);

  // takes all the errors that happened and returns a boolean array
  bool *getErrors();

  // takes all the errors that happened and returns an error code
  uint8_t getErrorCode(uint32_t currMicros);

  // generates a packet header for the sensor
  PacketHeader getHeader(packetType packetType_, uint8_t packetSize_,
                         uint32_t currMicros);

  // generate fake data when debugging the serial output without sensors
  float generateFakeData(float minValue, float maxValue,
                         uint32_t currMicros,
                         float offset = 0, uint32_t period = 5000000);
};