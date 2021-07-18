/*
 * serialOutput.h
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>

#include "globalVariables.h"
#include "flowCalculations.h"

#include "ADIS16470Wrapper.h"
#include "AISx120SXWrapper.h"
#include "HoneywellRscWrapper.h"
#include "MAX31855Wrapper.h"

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
  cursor_end = 48,  // 0
  cursor_beginning, // 1
  entire            // 2
} CLEAR_ARGUMENTS;

const static uint8_t HEADER_STYLE_LENGTH = 3;
const static DECORATOR_CODES HEADER_STYLE[HEADER_STYLE_LENGTH] =
    {bold, underline, magenta};
const static uint8_t ERROR_STYLE_LENGTH = 2;
const static DECORATOR_CODES ERROR_STYLE[ERROR_STYLE_LENGTH] = {bold, bg_red};

// sends the specified ANSI command to the console
void ansiCommand(COMMAND_CODES commandCode, char argument);

// decodes the command code enum into the actual character required
char decodeCommandCode(COMMAND_CODES commandCode);

// decorates the given text with the given decorators
void decorateText(const char *text, const DECORATOR_CODES *decoratorCodes,
                  uint8_t decoratorQty);

void setPosition(uint8_t row, uint8_t column);

void printErrors(uint8_t errorCode);

void outputSensorData(uint32_t currMicros,
                      ADIS16470Packet adis16470Packet,
                      AISx120SXPacket ais1120sxPacket,
                      HoneywellRSCPacket *rscPacket,
                      MAX31855Packet *maxPacket);