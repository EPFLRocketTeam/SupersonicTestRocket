/*
 * PushButtonArrayEvent.cpp
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 */

#pragma once
#include <Arduino.h>

// define the type of events that can happen
typedef enum
{
  NONE,            // no event
  GOOD_TRANSITION, // transition state to good state inside window
  BAD_TRANSITION,  // any other transition
  WINDOW_START,    // entered the window
  WINDOW_END       // left the window
} eventType;

// Events that can be triggered by a PushButtonArray
class PushButtonArrayEvent
{
private:
  // whether the event is deleted or not. using a flag because of static arrays
  bool deleted;
  uint8_t eventState;             // state to trigger the holding
  uint8_t eventNextState;         // next state to trigger the transition
  uint32_t eventWindowStart;      // when the event window starts
  uint32_t eventWindowEnd;        // when the event window ends
  eventType lastEventType = NONE; // last event type that happened

public:
  bool activated; // whether the event is activated or not

  // constructor
  PushButtonArrayEvent();

  // see if the event is deleted or not.
  bool deletedStatus();
  // Activate event with arguments
  void createEvent(uint8_t state, uint8_t nextState, uint32_t windowStart,
                   uint32_t windowEnd);
  void deleteEvent();
  // Check if there was an event
  eventType checkEvent(uint8_t lastState, uint8_t state,
                       uint32_t stateDuration);
};
