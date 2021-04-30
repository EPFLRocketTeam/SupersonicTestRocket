/*
 * PushButtonArrayEvent.h
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 */

#include "PushButtonArrayEvent.h"

// Default constructor with the event deleted (empty event)
PushButtonArrayEvent::PushButtonArrayEvent()
{
  deleteEvent();
}

bool PushButtonArrayEvent::deletedStatus()
{
  return deleted;
}

void PushButtonArrayEvent::createEvent(uint8_t state, uint8_t nextState,
                                       uint32_t windowStart,
                                       uint32_t windowEnd)
{
  activated = true;
  deleted = false;
  eventState = state;
  eventNextState = nextState;
  eventWindowStart = windowStart;
  eventWindowEnd = windowEnd;
}

// Delete the event by setting it to deleted
// and all of its elements to zero
void PushButtonArrayEvent::deleteEvent()
{
  activated = false;
  deleted = true;
  eventState = 0;
  eventWindowStart = 0;
  eventWindowEnd = 0;
  lastEventType = NONE;
}

eventType PushButtonArrayEvent::checkEvent(uint8_t lastState, uint8_t state,
                                           uint32_t stateDuration)
{
  if (activated)
  {

    if (lastState == eventState && state == eventNextState &&
        stateDuration >= eventWindowStart && stateDuration <= eventWindowEnd &&
        lastEventType != GOOD_TRANSITION) // released in window
    {
      lastEventType = GOOD_TRANSITION;
      return lastEventType;
    }
    else if (lastState == eventState && state == eventState &&
             stateDuration >= eventWindowStart &&
             stateDuration <= eventWindowEnd &&
             lastEventType != WINDOW_START) // holding enter window
    {
      lastEventType = WINDOW_START;
      return lastEventType;
    }
    else if (lastState == eventState && state == eventState &&
             stateDuration >= eventWindowEnd &&
             lastEventType != WINDOW_END) // holding exit window
    {
      lastEventType = WINDOW_END;
      return lastEventType;
    }
    else if (lastState != state)
    {
      lastEventType = BAD_TRANSITION;
      return lastEventType;
    }
    else
    {
      return NONE;
    }
  }
  else
  {
    return NONE;
  }
}