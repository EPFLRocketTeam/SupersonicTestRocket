/*
 * Logger.cpp
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#include "logging.hpp"

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
    counterFile.rewind(); // return to beginning so we don't append to file
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

// Deprecated function as too slow on Teensy
// // converts from a binary file of packets to CSV files for human reading
// bool binFileToCSV(FsFile &binFile)
// {
//   FsFile outFile;

//   // Open or create counter file.
//   if (!outFile.open("out2.csv", O_RDWR | O_CREAT | O_TRUNC))
//   {
//     Serial.println("Open out file failed.");
//     return;
//   }

//   // read the first byte of the next packet
//   while (uint8_t packetType = binFile.read())
//   {
//     if (packetType == -1)
//     {
//       Serial.println("There was an error reading the file.");
//       return;
//     }
//     uint8_t packetLength = binFile.read();
//     binFile.seekCur(-2); // go back two bytes since they were already read

//     // check if the read packet corresponds to any of the defined packed types
//     if (packetType == 1 && packetLength == sizeof(IMUPacket))
//     {
//     }
//     else if (packetType == 2 && packetLength == sizeof(AISx120SXPacket))
//     {
//       struct AISx120SXPacket packet;
//       binFile.read((uint8_t *)&packet, sizeof(packet));
//       outFile.printField(packet.header.timestamp, ',');
//       outFile.printField(packet.accelX, ',');
//       outFile.printField(packet.accelX, '\n');
//     }
//     else if (packetType == 3 && packetLength ==
//                                     sizeof(HoneywellRSCPressurePacket))
//     {
//     }
//     else if (packetType == 4 && packetLength == sizeof(HoneywellRSCTempPacket))
//     {
//     }
//     else if (packetType == 5 && packetLength == sizeof(ThermocouplePacket))
//     {
//     }
//     else if (packetLength == 0)
//     {
//       Serial.println("Packet length was zero. Likely reached EOF.");
//       return 0;
//     }
//     else
//     {
//       Serial.println("Could not read packet! Cannot continue.");
//       return 0;
//     }
//   }
//   outFile.truncate();
//   outFile.rewind();
//   outFile.close();
//   return 1;
// }