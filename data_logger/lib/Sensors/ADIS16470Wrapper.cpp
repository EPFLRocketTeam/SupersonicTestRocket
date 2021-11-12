/*
 * ADIS16470Wrapper.cpp
 *
 *  Created on: 2021-04-26
 *      Author: Joshua Cayetano-Emond
 */

#include "ADIS16470Wrapper.hpp"

// initialize the sensor count
uint8_t ADIS16470Wrapper::sensorQty = 0;

// ----- Constructor ----- //
ADIS16470Wrapper::
    ADIS16470Wrapper(int CS, int DR, int RST) : Sensor(sensorQty),
                                                DR_PIN(DR),
                                                adisObject(CS, DR, RST),
                                                lastPacket(getHeader(0))
{
  setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN, MEASUREMENT_INTERVAL,
                  true);
  sensorQty += 1;
  active = false;
}

// ----- Destructor ----- //
ADIS16470Wrapper::~ADIS16470Wrapper()
{
  sensorQty -= 1;
}

bool ADIS16470Wrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
  adisObject.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
  adisObject.regWrite(DEC_RATE, 0x00);  // Set digital filter
  adisObject.regWrite(FILT_CTRL, 0x04); // Set digital filter

  // Try to see if the ADIS is working
  for (uint32_t i = 0; i < attempts; i++)
  {
    // acquire some data
    uint16_t *wordBurstData;
    wordBurstData = adisObject.wordBurst(); // Read data and insert into array

    int16_t checksum = adisObject.checksum(wordBurstData); // get the checksum

    // get a zero vector to make sure the data we are getting isn't just zeros
    uint16_t zeros[sizeof(uint16_t) * ADIS16470::wordBurstLength] = {0};

    // checksum ok AND didn't read just zeros --> setup successful!
    if (wordBurstData[9] == checksum &&
        memcmp(wordBurstData, zeros, sizeof(uint16_t) * ADIS16470::wordBurstLength) != 0)
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
    allZeros = allZeros && lastPacket.getGyro(i) == 0; //  if gyro zero
    allZeros = allZeros && lastPacket.getAcc(i) == 0;  // if pressure zero
    if (lastPacket.getGyro(i) > GYRO_MAX ||
        lastPacket.getGyro(i) < GYRO_MIN)
    {
      return true;
    }
    if (lastPacket.getAcc(i) > ACC_MAX || lastPacket.getAcc(i) < ACC_MIN)
    {
      return true;
    }
  }
  allZeros = allZeros && lastPacket.getTemp() == 0; // check if zero
  if (lastPacket.getTemp() > TEMP_MAX || lastPacket.getTemp() < TEMP_MIN)
  {
    return true;
  }

  return allZeros;
}

ADIS16470Packet ADIS16470Wrapper::getPacket(uint32_t currMicros)
{
#ifdef DEBUG

  lastPacket.gyros[0] = generateFakeData(-2000, 2000, micros());
  lastPacket.gyros[1] = generateFakeData(-2000, 2000, micros(),
                                         500, 4800000);
  lastPacket.gyros[2] = generateFakeData(-2000, 2000, micros(),
                                         -200, 5200000);
  lastPacket.acc[0] = generateFakeData(-40, 40, micros());
  lastPacket.acc[1] = generateFakeData(-40, 40, micros(), 0, 4850000);
  lastPacket.acc[2] = generateFakeData(-40, 40, micros(), 1, 5250000);
  lastPacket.temp = generateFakeData(-5, 5, micros(), 23, 5000000);

#endif

  if (active)
  {
    // acquire the data
    uint16_t *wordBurstData;
    wordBurstData = adisObject.wordBurst(); // Read data and insert into array
    verifyCheckSum(wordBurstData);

    lastPacket.setXGyro(((int16_t)wordBurstData[1]) * GYRO_SENSITIVITY);
    lastPacket.setYGyro(((int16_t)wordBurstData[2]) * GYRO_SENSITIVITY);
    lastPacket.setZGyro(((int16_t)wordBurstData[3]) * GYRO_SENSITIVITY);
    lastPacket.setXAcc(((int16_t)wordBurstData[4]) * ACC_SENSITIVITY);
    lastPacket.setYAcc(((int16_t)wordBurstData[5]) * ACC_SENSITIVITY);
    lastPacket.setZAcc(((int16_t)wordBurstData[6]) * ACC_SENSITIVITY);
    lastPacket.setTemp(((int16_t)wordBurstData[7]) * TEMP_SENSITIVITY);
  }
  // check for errors and create the header
  lastPacket.updateHeader(getHeader(currMicros));

  return lastPacket;
}

PacketHeader ADIS16470Wrapper::getHeader(uint32_t currMicros)
{
  return Sensor::getHeader(ADIS16470_PACKET_TYPE,
                           sizeof(ADIS16470Body),
                           currMicros);
}