/*
 * ADIS16470Wrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "ADIS16470Wrapper.h"

// initialize the sensor count
uint8_t ADIS16470Wrapper::sensorQty = 0;

// constructor
ADIS16470Wrapper::
    ADIS16470Wrapper(int CS, int DR, int RST) : Sensor(sensorQty),
                                                DR_PIN(DR),
                                                adisObject(CS, DR, RST),
                                                lastPacket(getHeader(
                                                    ADIS16470_PACKET_TYPE,
                                                    sizeof(ADIS16470Packet),
                                                    0))
{
  setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN, MEASUREMENT_INTERVAL,
                  true);
  sensorQty += 1;
}

// destructor
ADIS16470Wrapper::~ADIS16470Wrapper()
{
  sensorQty -= 1;
}

bool ADIS16470Wrapper::setup(int attempts, int delayDuration)
{
  adisObject.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
  adisObject.regWrite(DEC_RATE, 0x00);  // Set digital filter
  adisObject.regWrite(FILT_CTRL, 0x04); // Set digital filter

  // Try to see if the ADIS is working
  for (int i = 0; i < attempts; i++)
  {
    // acquire some data
    uint16_t *wordBurstData;
    wordBurstData = adisObject.wordBurst(); // Read data and insert into array

    int16_t checksum = adisObject.checksum(wordBurstData); // get the checksum

    // get a zero vector to make sure the data we are getting isn't just zeros
    uint16_t zeros[sizeof(wordBurstData)] = {0};

    // checksum ok AND didn't read just zeros --> setup successful!
    if (wordBurstData[9] == checksum &&
        memcmp(wordBurstData, zeros, sizeof(wordBurstData)) != 0)
    {
      active = true;
      return active;
    }
    else // give it time before the next try
    {
      delay(delayDuration);
    }
  }
  active = false;
  return active; // setup was not succesful
}

uint8_t ADIS16470Wrapper::getSensorQty()
{
  return sensorQty;
}

bool ADIS16470Wrapper::isDue(uint32_t currMicros, volatile bool &triggeredDR)
{
  if (isDueByDR(currMicros, triggeredDR) || isDueByTime(currMicros))
  {
    prevMeasTime = currMicros;
    return true;
  }
  else
  {
    return false;
  }
}

bool ADIS16470Wrapper::verifyCheckSum(uint16_t sensorData[10])
{
  // calculate the checksum
  int16_t checksum = adisObject.checksum(sensorData);
  // compare it against the received checksum
  checksumError = sensorData[9] != checksum;

  return !checksumError;
}

// if the measurement is invalid
bool ADIS16470Wrapper::isMeasurementInvalid()
{
  bool allZeros = true; // if all values are equal to zero

  for (size_t i = 0; i < 3; i++)
  {
    allZeros = allZeros && lastSerialPacket.gyros[i] == 0; //  if gyro zero
    allZeros = allZeros && lastSerialPacket.acc[i] == 0;   // if pressure zero
    if (lastSerialPacket.gyros[i] > GYRO_MAX ||
        lastSerialPacket.gyros[i] < GYRO_MIN)
    {
      return true;
    }
    if (lastSerialPacket.acc[i] > ACC_MAX || lastSerialPacket.acc[i] < ACC_MIN)
    {
      return true;
    }
  }
  allZeros = allZeros && lastSerialPacket.temp == 0; // check if zero
  if (lastSerialPacket.temp > TEMP_MAX || lastSerialPacket.temp < TEMP_MIN)
  {
    return true;
  }

  return allZeros;
}

ADIS16470Packet ADIS16470Wrapper::getPacket(uint32_t currMicros)
{
  // acquire the data
  uint16_t *wordBurstData;
  wordBurstData = adisObject.wordBurst(); // Read data and insert into array

  verifyCheckSum(wordBurstData);

  // create and write the packet
  lastPacket = ADIS16470Packet(getHeader(ADIS16470_PACKET_TYPE,
                                         sizeof(ADIS16470Packet),
                                         currMicros),
                               wordBurstData);
  return lastPacket;
}

ADIS16470SerialPacket ADIS16470Wrapper::getSerialPacket(bool debug)
{
  if (debug)
  {
    lastSerialPacket.gyros[0] = generateFakeData(-2000, 2000, micros());
    lastSerialPacket.gyros[1] = generateFakeData(-2000, 2000, micros(),
                                                 500, 4800000);
    lastSerialPacket.gyros[2] = generateFakeData(-2000, 2000, micros(),
                                                 -200, 5200000);
    lastSerialPacket.acc[0] = generateFakeData(-40, 40, micros());
    lastSerialPacket.acc[1] = generateFakeData(-40, 40, micros(), 0, 4850000);
    lastSerialPacket.acc[2] = generateFakeData(-40, 40, micros(), 1, 5250000);
    lastSerialPacket.temp = generateFakeData(-5, 5, micros(), 23, 5000000);
  }
  else
  {
    lastSerialPacket.gyros[0] = ((int16_t)lastPacket.gyroX) * 0.1;
    lastSerialPacket.gyros[1] = ((int16_t)lastPacket.gyroY) * 0.1;
    lastSerialPacket.gyros[2] = ((int16_t)lastPacket.gyroZ) * 0.1;
    lastSerialPacket.acc[0] = ((int16_t)lastPacket.accX) * 0.00125;
    lastSerialPacket.acc[1] = ((int16_t)lastPacket.accY) * 0.00125;
    lastSerialPacket.acc[2] = ((int16_t)lastPacket.accZ) * 0.00125;
    lastSerialPacket.temp = ((int16_t)lastPacket.temp * 0.1);
  }
  memcpy(lastSerialPacket.errors, getErrors(), ERROR_TYPE_NUM);

  return lastSerialPacket;
}