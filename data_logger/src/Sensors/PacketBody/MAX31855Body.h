#pragma once

/// Describe the content of a MAX31855 packet
struct MAX31855Body
{
  float probeTemperature = 0;  ///< [degC]; 4 bytes
  float sensorTemperature = 0; ///< [degC]; 4 bytes
};