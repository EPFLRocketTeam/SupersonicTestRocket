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

void PushButtonArrayEvent::activateEvent()
{
  activated = true;
  lastEventType = NONE;
}

void PushButtonArrayEvent::deactivateEvent()
{
  activated = false;
  lastEventType = NONE;
}

bool PushButtonArrayEvent::activatedStatus()
{
  return activated;
}

void PushButtonArrayEvent::createEvent(uint8_t state, uint8_t nextState,
                                       uint32_t windowStart,
                                       uint32_t windowEnd)
{
  activateEvent();
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

bool PushButtonArrayEvent::deletedStatus()
{
  return deleted;
}

eventType PushButtonArrayEvent::checkEvent(uint8_t lastState, uint8_t state,
                                           uint32_t stateDuration)
{
  if (activated)
  {
    if (lastState != state)
    {
      if (lastState == eventState && state == eventNextState &&
          stateDuration >= eventWindowStart && stateDuration <= eventWindowEnd)
      { // released in window
        if (lastEventType != GOOD_TRANSITION)
        {
          lastEventType = GOOD_TRANSITION;
          return lastEventType;
        }
        else
        { // if transition already triggered
          return NONE;
        }
      }
      else
      {
        lastEventType = BAD_TRANSITION;
        return lastEventType;
      }
    }
    else if (lastState == eventState && state == eventState)
    {
      if (stateDuration >= eventWindowStart &&
          stateDuration < eventWindowEnd &&
          lastEventType != WINDOW_START && lastEventType != GOOD_TRANSITION)
      { // holding enter window
        lastEventType = WINDOW_START;
        return lastEventType;
      }
      else if (stateDuration >= eventWindowEnd &&
               lastEventType != WINDOW_END)
      { // holding exit window
        lastEventType = WINDOW_END;
        return lastEventType;
      }
      else
      { // if in wrong window or last event type already triggered
        return NONE;
      }
    }
    else
    { // if state transitions were not correct
      return NONE;
    }
  }
  else
  { // if not activated
    return NONE;
  }
}