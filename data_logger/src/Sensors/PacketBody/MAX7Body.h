#pragma once

/// Describe the content of a MAX7 packet
struct MAX7Body
{
  uint32_t latitude;  ///< [deg * 10^-7], 4 bytes
  uint32_t longitude; ///< [deg * 10^-7], 4 bytes
  uint32_t altitude;  ///< [mm], 4 bytes
};