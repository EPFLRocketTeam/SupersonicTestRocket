#pragma once

/// Describe the content of a ADIS16470 packet
struct ADIS16470Body
{
  float gyros[3] = {0}; ///< Angular velocities [deg/s] around X,Y,Z axis; 3 * 4 = 12 bytes
  float acc[3] = {0};   ///< Linear acceleration [g] along X,Y,Z axis; 3 * 4 = 12 bytes
  float temp = 0;       ///< Temperature [degC]; 4 bytes
};