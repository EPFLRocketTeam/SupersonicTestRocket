#pragma once

/// Describe the content of a AISx120SX packet
struct AISx120SXBody
{
  float accel[2] = {0}; ///< Linear acceleration [g] along X,Y axis; 2 * 4 = 8 bytes
};