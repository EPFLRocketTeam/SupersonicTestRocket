#pragma once 

/// Describe the content of a HoneywellRSC packet
struct HoneywellRSCBody
{
  /**
   * Using floats since the computation is hard to do after acquistion; 4 bytes
   * Temperature is in degC and pressure in bar
   */
  float measurement = 0;
};