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
  NONE,         // no event
  WINDOW_START, // entered the window
  WINDOW_END,   // left the window
  RELEASED      // released state inside window
} eventType;

// Events that can be triggered by a PushButtonArray
class PushButtonArrayEvent
{
private:
  bool activated;                 // whether the event is activated or not
  uint8_t eventState;             // state to trigger the event
  unsigned long eventWindowStart; // when the event window starts
  unsigned long eventWindowEnd;   // when the event window ends
  eventType lastEventType;        // last event type that happened

public:
  // constructor
  PushButtonArrayEvent();

  // Get the current state of the event (activated/deactivated)
  bool status();
  // Activate event with arguments
  void activate(uint8_t state, unsigned long windowStart,
                unsigned long windowEnd);
  void deactivate();
  // Check if there was an event
  eventType checkEvent(uint8_t state, unsigned long stateDuration,
                       bool released);
};