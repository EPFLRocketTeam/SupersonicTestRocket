/*
 * serialOutput.hpp
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>

#include "globalVariables.hpp"
#include "flowCalculations.h"

#include "ADIS16470Wrapper.hpp"
#include "AISx120SXWrapper.hpp"
#include "HoneywellRscWrapper.hpp"
#include "MAX31855Wrapper.hpp"

typedef enum
{
  decorate = 109,        // m
  up = 65,               // A
  down,                  // B
  right,                 // C
  left,                  // D
  next_line,             // E
  prev_line,             // F
  set_column,            // G
  clear_screen = 74,     // J
  clear_line,            // K
  save_position = 115,   // s
  recover_position = 117 // u
} COMMAND_CODES;

typedef enum
{
  reset = 0,
  bold = 1,
  underline = 4,
  reversed = 7,
  black = 30,
  red,
  green,
  yellow,
  blue,
  magenta,
  cyan,
  white,
  bg_black = 40,
  bg_red,
  bg_green,
  bg_yellow,
  bg_blue,
  bg_magenta,
  bg_cyan,
  bg_white,
} DECORATOR_CODES;

typedef enum
{
  cursor_end,       // 0
  cursor_beginning, // 1
  entire            // 2
} CLEAR_ARGUMENTS;

const static uint8_t HEADER_STYLE_LENGTH = 3;
const static DECORATOR_CODES HEADER_STYLE[HEADER_STYLE_LENGTH] =
    {bold, underline, magenta};
const static uint8_t ERROR_STYLE_LENGTH = 2;
const static DECORATOR_CODES ERROR_STYLE[ERROR_STYLE_LENGTH] = {bold, bg_red};

// sends the specified ANSI command to the console
const char *ansiCommand(COMMAND_CODES commandCode, uint8_t argument,
                        bool print = true);

// decorates the given text with the given decorators
const char *decorateText(const char *text,
                         const DECORATOR_CODES *decoratorCodes,
                         uint8_t decoratorQty,
                         bool print = true);

void setPosition(uint8_t row, uint8_t column);

class SerialMonitor
{
private:
  char messages[10][160];
  uint32_t messagesTimestamps[10];
  uint8_t pointer;

  void printErrors(const char *sensorName, uint8_t errorCode);
  void printReading(const char *readingName, float readingValue);
  void printMessages();

public:
  SerialMonitor();
  ~SerialMonitor();

  void writeMessage(const char *message,
                    uint32_t currMicros,
                    bool print = false);
  void outputSensorData(uint32_t currMicros,
                        ADIS16470Packet adis16470Packet,
                        AISx120SXPacket ais1120sxPacket,
                        HoneywellRSCPacket *rscPacket,
                        uint8_t rscQty,
                        MAX31855Packet *maxPacket,
                        uint8_t maxQty);
};
