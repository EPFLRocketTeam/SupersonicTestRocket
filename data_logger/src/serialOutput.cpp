/*
 * serialOutput.cpp
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#include "serialOutput.h"

void ansiCommand(COMMAND_CODES commandCode, uint8_t argument)
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
    ansiCommand(decorate, decoratorCodes[i]);
  }
  Serial.print(text);
  ansiCommand(decorate, reset); // resets the text style after the text was sent
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
  
  Serial.print("\u001b[2J"); // clear screen
  Serial.print("\u001b[H"); // cursor to home

  
  // Serial.println(sizeof(rscPacket));
  // Serial.println(sizeof(maxPacket));
  // unsigned char resetScreen[] = {27, '[', '2', 'J'};
  // Serial.write(resetScreen, 4);
  // unsigned char moveHome[] = {27, '[', 'H'};
  // Serial.write(moveHome, 3);
  //Serial.print("\u001b[2J");
  //ansiCommand(clear_screen, entire);

  Serial.println("HERMES Ground Station Monitor");
  Serial.print("  > Time (s): ");
  Serial.println(currMicros / 1000000., 3);
  Serial.println("");

  Serial.println("SENSORS");

  Serial.print("ADIS16470: ");
  Serial.print(adis16470Packet.errors[0]);
  Serial.print(", ");
  Serial.print(adis16470Packet.errors[1]);
  Serial.print(", ");
  Serial.print(adis16470Packet.errors[2]);
  Serial.print(", ");
  Serial.println(adis16470Packet.errors[3]);
  Serial.print("  > GyroX (deg/s): ");
  Serial.println(adis16470Packet.readings[0]);
  Serial.print("  > GyroY (deg/s): ");
  Serial.println(adis16470Packet.readings[1]);
  Serial.print("  > GyroZ (deg/s): ");
  Serial.println(adis16470Packet.readings[2]);
  Serial.print("  > AccelX (g): ");
  Serial.println(adis16470Packet.readings[3]);
  Serial.print("  > AccelY (g): ");
  Serial.println(adis16470Packet.readings[4]);
  Serial.print("  > AccelZ (g): ");
  Serial.println(adis16470Packet.readings[5]);
  Serial.print("  > Temp (degC): ");
  Serial.println(adis16470Packet.readings[6]);
  Serial.println("");

  Serial.print("AIS1120SX");
  Serial.print(ais1120sxPacket.errors[0]);
  Serial.print(ais1120sxPacket.errors[1]);
  Serial.print(ais1120sxPacket.errors[2]);
  Serial.println(ais1120sxPacket.errors[3]);
  Serial.print("  > AccelX (g): ");
  Serial.println(ais1120sxPacket.readings[0]);
  Serial.print("  > AccelY (g): ");
  Serial.println(ais1120sxPacket.readings[1]);
  Serial.println("");

  for (size_t i = 0; i < 2; i++)
  {
    Serial.print("RSC");
    Serial.print(i);
    Serial.print(rscPacket[0].errors[0]);
    Serial.print(rscPacket[0].errors[1]);
    Serial.print(rscPacket[0].errors[2]);
    Serial.println(rscPacket[0].errors[3]);
    Serial.print("  > Pressure (PSI): ");
    Serial.println(rscPacket[0].readings[0]);
    Serial.print("  > Temperature (degC): ");
    Serial.println(rscPacket[0].readings[1]);
    Serial.println("");
  }

  for (size_t i = 0; i < sizeof(maxPacket); i++)
  {
    Serial.print("MAX");
    Serial.print(i);
    Serial.print(maxPacket[0].errors[0]);
    Serial.print(maxPacket[0].errors[1]);
    Serial.print(maxPacket[0].errors[2]);
    Serial.println(maxPacket[0].errors[3]);
    Serial.print("  > Probe temperature (degC): ");
    Serial.println(maxPacket[0].readings[0]);
    Serial.print("  > Ambient temperature (degC): ");
    Serial.println(maxPacket[0].readings[1]);
    Serial.println("");
  }

  // printf(" %-4s| %-10s| %-5s|\n", "ID", "NAME", "AGE");
}