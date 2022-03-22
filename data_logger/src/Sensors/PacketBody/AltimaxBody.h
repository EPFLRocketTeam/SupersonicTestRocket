#pragma once

/// Describe the content of a Altimax pakcet
struct AltimaxBody
{
  /**
   * Altimax has 3 pins that can output data
   * The 4th boolean is to ensure proper structure alignment (Teensy is 32-bit)
   * 4 * 1 = 4 bytes
   */
  bool pinStates[4] = {0};
};