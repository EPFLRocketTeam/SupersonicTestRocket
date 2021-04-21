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
  WINDOW_START,    // entered the window
  WINDOW_END,      // left the window
  GOOD_TRANSITION, // transition state to good state inside window
  BAD_TRANSITION   // any other transition
} eventType;

// Events that can be triggered by a PushButtonArray
class PushButtonArrayEvent
{
private:
  bool activated;                 // whether the event is activated or not
  bool deleted;                   // whether the event is deleted or not
  uint8_t eventState;             // state to trigger the holding
  uint8_t eventNextState;         // next state to trigger the transition
  unsigned long eventWindowStart; // when the event window starts
  unsigned long eventWindowEnd;   // when the event window ends
  eventType lastEventType;        // last event type that happened

public:
  // constructor
  PushButtonArrayEvent();

  // Get the current state of the event (activated/deactivated)
  bool status();
  bool deletedStatus();
  // Activate event with arguments
  void activate(uint8_t state, uint8_t nextState, unsigned long windowStart,
                unsigned long windowEnd);
  void activate();
  void deactivate();
  void deleteEvent();
  // Check if there was an event
  eventType checkEvent(uint8_t state, uint8_t lastState,
                       unsigned long stateDuration);
};
