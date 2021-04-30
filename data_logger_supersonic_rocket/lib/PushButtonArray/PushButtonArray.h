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
  static const uint8_t BUTTON_NUM = 2;                      // number of buttons
  static const uint8_t STATE_NUM = BUTTON_NUM * BUTTON_NUM; // number of states
  static const uint8_t EVENT_NUM = 8;                       // max events

  uint32_t stateStartTime[STATE_NUM] = {0}; // moment entered state
  uint32_t stateEndTime[STATE_NUM] = {0};   // moment left state
  uint32_t stateDuration[STATE_NUM] = {0};  // how long was in state

  PushButtonArrayEvent Events[EVENT_NUM]; // the events associated to the array

  uint8_t state = 0;     // current state of button array
                         //    0--> button 0: LOW,  button 1: LOW
                         //    1--> button 0: HIGH, button 1: LOW
                         //    2--> button 0: LOW,  button 1: HIGH
                         //    3--> button 0: HIGH, button 1: HIGH
  uint8_t lastState = 0; // previous state of button array

public:
  // constructor
  PushButtonArray();

  uint8_t getState();

  // Gets the state of button array according to the individual button states
  uint8_t translateState(bool individualButtonStates[BUTTON_NUM]);

  uint8_t addEvent(uint8_t state, uint8_t nextState, uint32_t windowStart,
                   uint32_t windowEnd);
  void activateEvent(uint8_t idx);
  void deactivateEvent(uint8_t idx);
  void deleteEvent(uint8_t idx);

  // Check if there are any events that just happened
  eventOutput checkEvents(uint32_t currMillis,
                          bool individualButtonStates[BUTTON_NUM]);
};