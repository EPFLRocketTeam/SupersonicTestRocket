/*
 * PushButtonArray.h An array of two buttons to monitor and perform events.
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 *      Inspired by PushButtonClicks library by Alireza Sadri
 *          Available at https://github.com/ARSadri/PushButtonClicks
 */

#pragma once
#include <Arduino.h>
#include "PushButtonArrayEvent.h"

struct eventOutput
{
  uint8_t triggeredEvent;
  eventType triggeredEventType;
};

// Push button array of two buttons that reacts to certain events.
// Supports up to 8 events. Number of events supported can easily be changed.
//
// TODO: Add support for more than two buttons.
class PushButtonArray
{
private:
  static const uint8_t buttonNum = 2;                    // number of buttons
  static const uint8_t stateNum = buttonNum * buttonNum; // number of states
  static const uint8_t eventNum = 8;                     // max number of events

  unsigned long stateStartTime[stateNum] = {0}; // moment entered state
  unsigned long stateEndTime[stateNum] = {0};   // moment left state
  unsigned long stateDuration[stateNum] = {0};  // how long was in state

  PushButtonArrayEvent Events[eventNum]; // the events associated to the array
  uint8_t state = 0;                     // current state of button array
                                         //    0--> button 0: LOW,  button 1: LOW
                                         //    1--> button 0: HIGH, button 1: LOW
                                         //    2--> button 0: LOW,  button 1: HIGH
                                         //    3--> button 0: HIGH, button 1: HIGH
  uint8_t lastState = 0;                 // previous state of button array

public:
  // constructor
  PushButtonArray();

  uint8_t getState();

  // Gets the state of button array according to the individual button states
  uint8_t translateState(bool individualButtonStates[buttonNum]);

  uint8_t addEvent(uint8_t state, uint8_t nextState, unsigned long windowStart,
                   unsigned long windowEnd);
  void activateEvent(uint8_t idx);
  void deactivateEvent(uint8_t idx);
  void deleteEvent(uint8_t idx);

  // Check if there are any events that just happened
  eventOutput checkEvents(bool individualButtonStates[buttonNum]);
};