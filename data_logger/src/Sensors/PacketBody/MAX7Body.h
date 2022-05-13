#pragma once

/// Describe the content of a MAX7 packet
struct MAX7Body
{
  float latitude;  ///< [deg], 4 bytes
  float longitude; ///< [deg], 4 bytes
  float altitude;  ///< [m], 4 bytes
};