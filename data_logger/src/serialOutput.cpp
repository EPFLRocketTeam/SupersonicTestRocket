/*
 * serialOutput.cpp
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#include "serialOutput.h"

// printf(" %-4s| %-10s| %-5s|\n", "ID", "NAME", "AGE");

void ansiCommand(uint8_t argument, COMMAND_CODES commandCode)
{
  Serial.print("\u001b[");
  Serial.print(argument);
  Serial.print(decodeCommandCode(commandCode));
}

char decodeCommandCode(COMMAND_CODES commandCode)
{
  char commandChar;
  switch (commandCode)
  {
  case decorate:
    commandChar = 'm';
  case up:
    commandChar = 'A';
  case down:
    commandChar = 'B';
  case right:
    commandChar = 'C';
  case left:
    commandChar = 'D';
  case next_line:
    commandChar = 'E';
  case prev_line:
    commandChar = 'F';
  case set_column:
    commandChar = 'G';
  case clear_screen:
    commandChar = 'J';
  case clear_line:
    commandChar = 'K';
  }
  return commandChar;
}

void decorateText(char *text, DECORATOR_CODES *decoratorCodes)
{
  // applies all the listed decorators
  for (size_t i = 0; i < sizeof(decoratorCodes); i++)
  {
    ansiCommand(decoratorCodes[i], decorate);
  }
  Serial.print(text);
  ansiCommand(reset, decorate); // resets the text style after the text was sent
}

void setPosition(uint8_t row, uint8_t column)
{
  Serial.print("\u001b[");
  Serial.print(row);
  Serial.print(";");
  Serial.print(column);
  Serial.print("H");
}

void savePosition()
{
  Serial.print("\u001b[{s}");
}

void recoverPosition()
{
  Serial.print("\u001b[{u}");
}

void outputSensorData(uint32_t currMicros, serialPacket adis16470Packet,
                      serialPacket ais1120sxPacket, serialPacket *rscPacket,
                      serialPacket *maxPacket)
{
  
}