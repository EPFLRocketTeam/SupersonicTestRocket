/*
 * Logger.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "logging.h"

void setupLoggingFile(FsFile &loggingFile,
                      RingBuf<FsFile, RING_BUF_CAPACITY> &rb)
{
  FsFile counterFile;
  uint16_t counterVal = 0;

  // Open or create counter file.
  if (!counterFile.open(COUNTER_FILENAME, O_RDWR | O_CREAT))
  {
    Serial.println("Open counter file failed.");
    return;
  }

  // if the file is not empty, get the current count
  // else start at zero
  if (counterFile.size() != 0)
  {
    counterFile.read(&counterVal, 2);
    counterFile.seek(0); // return to beginning so we don't append to file
  }

  // get the name of the logging file
  char fileName[16]; // 6 digits + 4 characters + cr + lf + 4 extra bytes
  snprintf(fileName, sizeof(fileName), "%06d.dat", counterVal);

  // write the count for the next logging file
  counterVal++;
  counterFile.write((const uint8_t *)&counterVal, 2);
  counterFile.close();

  // Open or create logging file.
  if (!loggingFile.open(fileName, O_RDWR | O_CREAT | O_TRUNC))
  {
    Serial.println("Open logging file failed.");
    return;
  }

  // File must be pre-allocated to avoid huge
  // delays searching for free clusters.
  if (!loggingFile.preAllocate(LOG_FILE_SIZE))
  {
    Serial.println("preAllocate failed.");
    loggingFile.close();
    return;
  }

  // initialize the RingBuf.
  rb.begin(&loggingFile);
}

AIS1120SXPacket encodeAIS1120SXPacket(unsigned long currMicros, uint16_t accel)
{
  AIS1120SXPacket packet;
  Serial.println(sizeof(packet));
  Serial.println(sizeof(currMicros));
  Serial.println(sizeof(accel));
  packet.timestamp = currMicros;
  packet.reading = accel;
  Serial.println(sizeof(packet));
  return packet;
}