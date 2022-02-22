/*
 * AltimaxWrapper.cpp
 *
 *  Created on: 2021-11-20
 *      Author: Joshua Cayetano-Emond
 */

#include "AltimaxWrapper.hpp"

// initialize the sensor count
uint8_t AltimaxWrapper::sensorQty = 0;

// ----- constructors ----- //
AltimaxWrapper::AltimaxWrapper(uint8_t PIN_0_, uint8_t PIN_1_, uint8_t PIN_2_)
    : Sensor(sensorQty),
      PIN_0(PIN_0_),
      PIN_1(PIN_1_),
      PIN_2(PIN_2_),
      lastPacket(getHeader(0))
{
  sensorQty += 1;
  active = false;
}

// ----- destructor ----- //
AltimaxWrapper::~AltimaxWrapper()
{
  sensorQty -= 1;
}

bool AltimaxWrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
  // Setup the timing parameters

  // See if the Altimax is working properly
  // Its pins should all be low
  for (uint32_t i = 0; i < attempts; i++)
  {
    bool pin0state = digitalRead(PIN_0);
    bool pin1state = digitalRead(PIN_1);
    bool pin2state = digitalRead(PIN_2);

    if (!(pin0state || pin1state || pin2state)) // all pins should be low
    {
      active = true;
      return active;
    }
    else // give it time before the next try
    {
      delay(delayDuration);
    }
  }
  active = false;
  return active; // setup was not succesful
}

uint8_t AltimaxWrapper::getSensorQty()
{
  return sensorQty;
}

bool AltimaxWrapper::isDue(uint32_t currMicros, volatile bool &triggeredDR)
{
  if (isDueByDR(currMicros, triggeredDR))
  {
    prevMeasTime = currMicros;
    return true;
  }
  else
  {
    return false;
  }
}

bool AltimaxWrapper::isMeasurementInvalid()
{
  // should not be more than 1 pin high
  // a && (b || c) || (b && c) checks if at least two out of 3 are high
  if (lastPacket.getPin0state() &&
          (lastPacket.getPin1state() || lastPacket.getPin2state()) ||
      (lastPacket.getPin1state() && lastPacket.getPin2state()))
  {
    return true;
  }
  return false;
}

AltimaxPacket *AltimaxWrapper::getPacket(uint32_t currMicros)
{
  // update the error on the packet
  lastPacket.updateHeader(getHeader(currMicros));
#ifdef DEBUG
  lastPacket.
// when not debugging readings are updated in isDue()
#endif

      lastPacket.setPin0state(digitalRead(PIN_0));
  lastPacket.setPin1state(digitalRead(PIN_1));
  lastPacket.setPin2state(digitalRead(PIN_2));

  return &lastPacket;
}

PacketHeader AltimaxWrapper::getHeader(uint32_t currMicros)
{
  return Sensor::getHeader(ALTIMAX_PACKET_TYPE,
                           sizeof(AltimaxBody),
                           currMicros);
}
