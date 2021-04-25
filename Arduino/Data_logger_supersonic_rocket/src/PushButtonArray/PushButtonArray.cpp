/*
 * PushButtonArray.cpp
 *
 *  Created on: 2021-04-21
 *      Author: Joshua Cayetano-Emond
 */

#include "PushButtonArray.h"

//The constructor
PushButtonArray::PushButtonArray()
{
}

uint8_t PushButtonArray::getState()
{
	return state;
}

uint8_t PushButtonArray::translateState(bool individualButtonStates[BUTTON_NUM])
{
	if (individualButtonStates[0] == 0 && individualButtonStates[1] == 0)
	{
		return 0;
	}
	else if (individualButtonStates[0] == 1 && individualButtonStates[1] == 0)
	{
		return 1;
	}
	else if (individualButtonStates[0] == 0 && individualButtonStates[1] == 1)
	{
		return 2;
	}
	else if (individualButtonStates[0] == 1 && individualButtonStates[1] == 1)
	{
		return 3;
	}
	else
	{
		return -1; // error
	}
}

uint8_t PushButtonArray::addEvent(uint8_t state, uint8_t nextState,
																	uint32_t windowStart,
																	uint32_t windowEnd)
{
	int i;
	for (i = 0; i <= 8; i++) // find first available event
	{
		if (i == 8) // there are no empty events
		{
			Serial.println("No available events left!");
			return -1;
		}
		if (Events[i].deletedStatus())
		// event was empty, will use it
		{
			break;
		}
	}

	// assign the new event to the empty event that was just found
	Events[i].createEvent(state, nextState, windowStart, windowEnd);
	return i;
}

void PushButtonArray::deleteEvent(uint8_t idx)
{
	Events[idx].deleteEvent();
}

void PushButtonArray::activateEvent(uint8_t idx)
{
	Events[idx].activated = true;
}

void PushButtonArray::deactivateEvent(uint8_t idx)
{
	Events[idx].activated = false;
}

eventOutput PushButtonArray::checkEvents(bool individualButtonStates[BUTTON_NUM])
{
	state = translateState(individualButtonStates);
	uint32_t durationToSend;

	if (state != lastState) // state changed
	{
		// update the state times
		// start the current state timer
		stateStartTime[state] = millis();
		stateDuration[state] = 0;
		// end the previous state timer
		stateEndTime[lastState] = stateStartTime[state];
		// calculate duration of previous state
		stateDuration[lastState] = stateEndTime[lastState] -
															 stateStartTime[lastState];

		// the duration to send to the event checker
		durationToSend = stateDuration[lastState];
	}
	else
	{
		// updates the timer of the current state of the button array
		stateDuration[state] = millis() - stateStartTime[state];

		// the duration to send to the event checker
		durationToSend = stateDuration[state];
	}

	// Check if any events were triggered
	eventOutput output;					// prepare the output structure
	output.triggeredEvent = -1; // no events triggered by default

	for (int i = 0; i < 8; i++) // go through all the events
	{
		if (!Events[i].activated)
		{ // event is not activate, skip it.
			continue;
		}
		output.triggeredEventType =
				Events[i].checkEvent(state, lastState, durationToSend);
		if (output.triggeredEventType != NONE) // a valid event happened
		{
			output.triggeredEvent = i;
			break; // so only the first detected event gets sent back
		}
	}

	lastState = state;
	return output;
}