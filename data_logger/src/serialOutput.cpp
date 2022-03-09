/*
 * serialOutput.cpp
 *
 *  Created on: 2021-07-11
 *      Author: Joshua Cayetano-Emond
 */

#include "serialOutput.hpp"

const char *ansiCommand(COMMAND_CODES commandCode, uint8_t argument,
                        bool print)
{
  static char buffer[12] = {0};
  strcpy(buffer, "\u001b[");
  // determine if the argument should be converted to a
  // char array representing the int or just ignored
  if (commandCode != save_position && commandCode != recover_position)
  {
    char argBuffer[8];
    itoa(argument, argBuffer, 10);
    strcat(buffer, argBuffer);
  }
  char commandCodeChar = (char)commandCode;
  strcat(buffer, &commandCodeChar);
  if (print)
  {
    Serial.write(buffer);
  }
  return buffer;
}

const char *decorateText(const char *text,
                         const DECORATOR_CODES *decoratorCodes,
                         uint8_t decoratorQty, bool print)
{
  // 20 columns of 8 characters + null terminator
  static char decoratedTextBuffer[161] = {0};
  strcpy(decoratedTextBuffer, ""); // make string empty since it's static 
  // applies all the listed decorators
  for (size_t i = 0; i < decoratorQty; i++)
  {
    strcat(decoratedTextBuffer, ansiCommand(decorate,
                                            decoratorCodes[i],
                                            false));
  }
  strcat(decoratedTextBuffer, text);
  // resets the text style after the text was sent
  strcat(decoratedTextBuffer, ansiCommand(decorate, reset, false));

  if (print)
  {
    Serial.write(decoratedTextBuffer);
  }
  return decoratedTextBuffer;
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

SerialMonitor::SerialMonitor() : messages{0},
                                 messagesTimestamps{0},
                                 pointer(0)
{
  // do stuff
}

SerialMonitor::~SerialMonitor()
{
  // do stuff
}

void SerialMonitor::printErrors(const char *sensorName, uint8_t errorCode)
{
  char buffer[161]; // 20 columns of 8 characters + null character

  sprintf(buffer, "%-24s",
          decorateText(sensorName, HEADER_STYLE, HEADER_STYLE_LENGTH, false));

  for (size_t i = 0; i < ERROR_TYPE_NUM; i++)
  {
    char errorMessage[16];
    if (bitRead(errorCode, 7 - i))
    { // there is an error
      strcpy(errorMessage,
             decorateText("ERROR", ERROR_STYLE, ERROR_STYLE_LENGTH, false));
    }
    else
    {
      strcpy(errorMessage, "Nominal");
    }
    sprintf(buffer, "%s%-16s", buffer, errorMessage);
  }
  Serial.println(buffer);
}

void SerialMonitor::printReading(const char *readingName, float readingValue)
{
  char buffer[41] = {0};
  char readingValueString[16] = {0};
  dtostrf(readingValue, 16, 3, readingValueString);
  sprintf(buffer, "%8s%-16.16s%16.16s", "", readingName, readingValueString);
  Serial.println(buffer);
}

void SerialMonitor::printMessages()
{
  decorateText("Messages\n", HEADER_STYLE, HEADER_STYLE_LENGTH);
  for (size_t i = 0; i < 10; i++)
  {
    uint8_t index = (pointer + i) % 10;
    if (strlen(messages[index]) != 0)
    {
      char buffer[161] = {0};
      char timestamp[16] = {0};
      dtostrf(messagesTimestamps[index] / 1000000., 13, 3, timestamp);
      sprintf(buffer, "\t[%s] %s", timestamp, messages[index]);
      Serial.println(buffer);
    }
  }
}

void SerialMonitor::writeMessage(const char *message,
                                 uint32_t currMicros,
                                 bool print)
{
  strcpy(messages[pointer], message);
  messagesTimestamps[pointer] = currMicros;
  if (print)
  {
    char buffer[161] = {0};
    char timestamp[16] = {0};
    dtostrf(messagesTimestamps[pointer] / 1000000., 13, 3, timestamp);
    sprintf(buffer, "\t[%s] %s", timestamp, messages[pointer]);
    Serial.println(buffer);
  }
  pointer = (pointer + 1) % 10;
}

void SerialMonitor::outputSensorData(uint32_t currMicros,
                                     ADIS16470Packet adis16470Packet,
                                     AISx120SXPacket ais1120sxPacket,
                                     HoneywellRSCPacket *rscPacket,
                                     uint8_t rscQty,
                                     MAX31855Packet *maxPacket,
                                     uint8_t maxQty)
{
  float mach = calcMachFromPressure(rscPacket[2 * staticRSC].measurement,
                                    rscPacket[2 * totalRSC].measurement);
  float staticTemperature =
      calcStaticTemperature(mach, maxPacket[TAT_TC].probeTemperature);
  float airspeed = calcAirspeed(mach, staticTemperature);
  float altitude = calcAltitude(rscPacket[staticRSC].measurement * PSI_TO_PA);

  ansiCommand(clear_screen, entire);
  ansiCommand(down, 1);
  setPosition(0, 0);

  // print the raw readings
  Serial.write((const uint8_t *)&adis16470Packet, sizeof(adis16470Packet));
  ansiCommand(clear_line, cursor_beginning);
  setPosition(0, 0);
  Serial.write((const uint8_t *)&ais1120sxPacket, sizeof(ais1120sxPacket));
  ansiCommand(clear_line, cursor_beginning);
  setPosition(0, 0);

  for (size_t i = 0; i < rscQty; i++)
  {
    Serial.write((const uint8_t *)&rscPacket[2 * i], sizeof(rscPacket[2 * i]));
    ansiCommand(clear_line, cursor_beginning);
    setPosition(0, 0);
    Serial.write((const uint8_t *)&rscPacket[2 * i + 1],
                 sizeof(rscPacket[2 * i]));
    ansiCommand(clear_line, cursor_beginning);
    setPosition(0, 0);
  }

  for (size_t i = 0; i < maxQty; i++)
  {
    Serial.write((const uint8_t *)&maxPacket[i], sizeof(maxPacket[i]));
    ansiCommand(clear_line, cursor_beginning);
    setPosition(0, 0);
  }

  // print human-friendly text
  decorateText("HERMES Ground Station Monitor\n", HEADER_STYLE,
               HEADER_STYLE_LENGTH);

  // char bufferErrorHeader[161] = {0};
  // char bufferHeaders[161] = {0};
  // sprintf(bufferErrorHeader, "%72s%-6s", "",
  //         decorateText("Errors", HEADER_STYLE, HEADER_STYLE_LENGTH, false));
  // sprintf(bufferHeaders,
  //         "%8s%-16s%-16s%-16s%-16s%-16s%-16s%-16s%-16s", "",
  //         decorateText("Variable", HEADER_STYLE, HEADER_STYLE_LENGTH, false),
  //         decorateText("Value", HEADER_STYLE, HEADER_STYLE_LENGTH, false),
  //         decorateText("Meas late", HEADER_STYLE, HEADER_STYLE_LENGTH, false),
  //         decorateText("Beat skipped", HEADER_STYLE,
  //                      HEADER_STYLE_LENGTH, false),
  //         decorateText("DR not ready", HEADER_STYLE,
  //                      HEADER_STYLE_LENGTH, false),
  //         decorateText("Checksum error", HEADER_STYLE,
  //                      HEADER_STYLE_LENGTH, false),
  //         decorateText("Invalid meas", HEADER_STYLE,
  //                      HEADER_STYLE_LENGTH, false),
  //         decorateText("Error latch", HEADER_STYLE,
  //                      HEADER_STYLE_LENGTH, false));
  // Serial.println(bufferErrorHeader);
  // Serial.println(bufferHeaders);

  printReading("Time (s):", currMicros / 1000000);
  printReading("Altitude (m):", altitude);
  printReading("Airspeed (m/s):", airspeed);
  Serial.println("");

  printErrors("ADIS16470", adis16470Packet.header.errorCode);
  printReading("GyroX (deg/s):", adis16470Packet.gyros[0]);
  printReading("GyroY (deg/s):", adis16470Packet.gyros[1]);
  printReading("GyroZ (deg/s):", adis16470Packet.gyros[2]);
  printReading("AccelX (g):", adis16470Packet.acc[0]);
  printReading("AccelY (g):", adis16470Packet.acc[1]);
  printReading("AccelZ (g):", adis16470Packet.acc[2]);
  printReading("Temp (degC):", adis16470Packet.temp);

  printErrors("AIS1120SX", ais1120sxPacket.header.errorCode);
  printReading("AccelX (g):", ais1120sxPacket.accel[0]);
  printReading("AccelY (g):", ais1120sxPacket.accel[1]);
  Serial.println("");

  for (size_t i = 0; i < rscQty; i++)
  {
    char buffer[8];
    sprintf(buffer, "RSC%u", i);
    printErrors(buffer, rscPacket[2 * i].header.errorCode);
    printReading("Pressure (PSI):", rscPacket[2 * i].measurement);
    printReading("Temp (degC):", rscPacket[2 * i + 1].measurement);
  }
  Serial.println("");

  for (size_t i = 0; i < maxQty; i++)
  {
    char buffer[8];
    sprintf(buffer, "MAX%u", i);
    printErrors(buffer, maxPacket[i].header.errorCode);
    printReading("Probe T (degC):", maxPacket[i].probeTemperature);
    printReading("Sensor T (degC):", maxPacket[i].sensorTemperature);
  }
  Serial.println("");

  printMessages();
}