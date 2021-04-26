/*
 * logging.h
 *
 *  Created on: 2021-04-22
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

// SdFat
//    Available at: https://github.com/greiman/SdFat
//    Also available from the library manager.
#include <SdFat.h>
#include <RingBuf.h>

#define SD_CONFIG SdioConfig(FIFO_SDIO)

// Size to log 64 byte lines at 2000Hz for more than sixty minutes.
#define LOG_FILE_SIZE 64 * 2000 * 60
// Space to hold 64 byte lines at 2000Hz for more than 1s.
#define RING_BUF_CAPACITY 64 * 2000 * 1
// Counter for the logging file name
#define COUNTER_FILENAME "counter.dat"

void setupLoggingFile(FsFile &loggingFile,
                      RingBuf<FsFile, RING_BUF_CAPACITY> &rb);

// Deprecated function as too slow on a Teensy
// converts from a binary file of packets to a CSV file for human reading
//bool binFileToCSV(FsFile &binFile);