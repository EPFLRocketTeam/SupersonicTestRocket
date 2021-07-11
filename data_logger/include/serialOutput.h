/*
 * serialOutput.h
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#pragma once

#include <Arduino.h>

typedef enum
{
  decorate,
  up,
  down,
  right,
  left,
  next_line,
  prev_line,
  set_column,
  clear_screen,
  clear_line
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

struct decodedPacket
{
  bool measSkippedBeat;
  bool skippedBeat;
  bool drNoTrigger;
  bool checksumError;

  float *readings;
};

// sends the specified ANSI command to the console
void ansiCommand(uint8_t argument, COMMAND_CODES commandCode);

// decodes the command code enum into the actual character required
char decodeCommandCode(COMMAND_CODES commandCode);

// decorates the given text with the given decorators
void decorateText(char *text, DECORATOR_CODES *decoratorCodes);

void setPosition(uint8_t row, uint8_t column);

void savePosition();

void recoverPosition();

void outputSensorData(uint32_t currMicros, decodedPacket adis16470Packet,
                      decodedPacket ais1120sxPacket, decodedPacket *rscPacket,
                      decodedPacket *maxPacket);