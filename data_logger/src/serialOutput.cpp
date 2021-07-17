/*
 * serialOutput.cpp
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#include "serialOutput.h"

void ansiCommand(COMMAND_CODES commandCode, char argument)
{
  Serial.write("\u001b["); // escape character
  // determine if the argument should be interpreted literally, converted to a
  // char array representing the int or just not used
  if (commandCode == clear_screen || commandCode == clear_line)
  {
    Serial.write(argument);
  }
  else if (commandCode != save_position && commandCode != recover_position)
  {
    char argBuffer[4];
    itoa(argument, argBuffer, 10);
    Serial.write(argBuffer);
  }
  Serial.write((char)commandCode);
}

void decorateText(const char *text, const DECORATOR_CODES *decoratorCodes,
                  uint8_t decoratorQty)
{
  // applies all the listed decorators
  for (size_t i = 0; i < decoratorQty; i++)
  {
    ansiCommand(decorate, decoratorCodes[i]);
  }
  Serial.write(text);
  ansiCommand(decorate, reset); // resets the text style after the text was sent
}

void setPosition(uint8_t row, uint8_t column)
{
  char rows[4];
  char columns[4];
  itoa(row, rows, 10);
  itoa(column, columns, 10);
  Serial.write("\u001b[");
  Serial.write(rows);
  Serial.write(";");
  Serial.write(columns);
  Serial.write("H");
}

void printErrors(bool errors[ERROR_TYPE_NUM])
{
  Serial.print("\tErrors\t\t");
  for (size_t i = 0; i < ERROR_TYPE_NUM; i++)
  {
    Serial.print("\t\t");
    Serial.print(errors[i]);
    if (errors[i])
    { // there is an error
      decorateText("ERROR", ERROR_STYLE, ERROR_STYLE_LENGTH);
    }
    else
    {
      Serial.print("Nominal");
    }
  }
  Serial.println("");
}

void outputSensorData(uint32_t currMicros,
                      ADIS16470SerialPacket adis16470Packet,
                      AISx120SXSerialPacket ais1120sxPacket,
                      HoneywellRSCSerialPacket *rscPacket,
                      MAX31855SerialPacket *maxPacket)
{
  ansiCommand(clear_screen, entire);
  setPosition(0, 0);

  decorateText("HERMES Ground Station Monitor\n", HEADER_STYLE,
               HEADER_STYLE_LENGTH);

  decorateText("\t\t\t\t\t\t\t\tErrors\n", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tVariable", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\t\tValue", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tMeas late", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tBeat skipped", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tDR not ready", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tChecksum error", HEADER_STYLE, HEADER_STYLE_LENGTH);
  decorateText("\tInvalid meas\n", HEADER_STYLE, HEADER_STYLE_LENGTH);

  Serial.print("\tTime (s):\t\t");
  Serial.println(currMicros / 1000000., 3);
  Serial.println("");

  decorateText("ADIS16470\n", HEADER_STYLE, HEADER_STYLE_LENGTH);
  printErrors(adis16470Packet.errors);
  Serial.print("\tGyroX (deg/s):\t\t");
  Serial.println(adis16470Packet.gyros[0]);
  Serial.print("\tGyroY (deg/s):\t\t");
  Serial.println(adis16470Packet.gyros[1]);
  Serial.print("\tGyroZ (deg/s):\t\t");
  Serial.println(adis16470Packet.gyros[2]);
  Serial.print("\tAccelX (g):\t\t");
  Serial.println(adis16470Packet.acc[0]);
  Serial.print("\tAccelY (g):\t\t");
  Serial.println(adis16470Packet.acc[1]);
  Serial.print("\tAccelZ (g):\t\t");
  Serial.println(adis16470Packet.acc[2]);
  Serial.print("\tTemp (degC):\t\t");
  Serial.println(adis16470Packet.temp);

  decorateText("AIS1120SX\n", HEADER_STYLE, HEADER_STYLE_LENGTH);
  printErrors(ais1120sxPacket.errors);
  Serial.print("\tAccelX (g):\t\t");
  Serial.println(ais1120sxPacket.acc[0]);
  Serial.print("\tAccelY (g):\t\t");
  Serial.println(ais1120sxPacket.acc[1]);
  Serial.println("");

  for (size_t i = 0; i < 2; i++)
  {
    char buffer[4];
    itoa(i, buffer, 10);
    decorateText("RSC", HEADER_STYLE, HEADER_STYLE_LENGTH);
    decorateText(buffer, HEADER_STYLE, HEADER_STYLE_LENGTH);
    Serial.println("");
    printErrors(rscPacket[i].errors);
    Serial.print("\tPressure (PSI):\t\t");
    Serial.println(rscPacket[i].pressure);
    Serial.print("\tTemperature (degC):\t");
    Serial.println(rscPacket[i].temp);
  }
  Serial.println("");

  for (size_t i = 0; i < sizeof(maxPacket); i++)
  {
    char buffer[4];
    itoa(i, buffer, 10);
    decorateText("MAX", HEADER_STYLE, HEADER_STYLE_LENGTH);
    decorateText(buffer, HEADER_STYLE, HEADER_STYLE_LENGTH);
    Serial.println("");
    printErrors(maxPacket[i].errors);
    Serial.print("\tProbe temp (degC):\t");
    Serial.println(maxPacket[i].probeTemperature);
    Serial.print("\tAmbient temp (degC):\t");
    Serial.println(maxPacket[i].sensorTemperature);
  }
}