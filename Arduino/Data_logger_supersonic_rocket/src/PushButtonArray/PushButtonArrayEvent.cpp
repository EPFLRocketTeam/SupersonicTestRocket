/*
 * PushButtonArrayEvent.h
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 */

#include "PushButtonArrayEvent.h"

// Default constructor with the event deactivated
PushButtonArrayEvent::PushButtonArrayEvent()
{
  deactivate();
}

bool PushButtonArrayEvent::status()
{
  return activated;
}

void PushButtonArrayEvent::activate(uint8_t state,
                                    unsigned long windowStart,
                                    unsigned long windowEnd)
{
  activated = true;
  eventState = state;
  eventWindowStart = windowStart;
  eventWindowEnd = windowEnd;
}

// Deactivate the event by setting it to deactivated
// and all of its elements to zero
void PushButtonArrayEvent::deactivate()
{
  activated = false;
  eventState = 0;
  eventWindowStart = 0;
  eventWindowEnd = 0;
  lastEventType = NONE;
}

eventType PushButtonArrayEvent::checkEvent(uint8_t state,
                                           unsigned long stateDuration,
                                           bool released)
{
  if (state == eventState && stateDuration >= eventWindowStart &&
      stateDuration <= eventWindowEnd &&
      released && lastEventType != RELEASED) // released in window
  {
    lastEventType = RELEASED;
    return lastEventType;
  }
  else if (state == eventState && stateDuration >= eventWindowStart &&
           stateDuration <= eventWindowEnd &&
           !released && lastEventType != WINDOW_START) // holding enter window
  {
    lastEventType = WINDOW_START;
    return lastEventType;
  }
  else if (state == eventState && stateDuration >= eventWindowEnd &&
           !released && lastEventType != WINDOW_END) // holding exit window
  {
    lastEventType = WINDOW_END;
    return lastEventType;
  }
  else
  {
    return NONE;
  }
}