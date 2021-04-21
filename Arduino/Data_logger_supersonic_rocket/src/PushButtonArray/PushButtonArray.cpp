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
	state = 0;
	lastState = 0;
}

uint8_t PushButtonArray::getState()
{
	return state;
}

uint8_t PushButtonArray::translateState(bool individualButtonStates[buttonNum])
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

uint8_t PushButtonArray::addEvent(uint8_t state, unsigned long windowStart,
																	unsigned long windowEnd)
{
	int i;
	for (i = 0; i <= 8; i++) // find first available event
	{
		if (i == 8) // there are no empty events
		{
			Serial.println("No available events left!");
			return -1;
		}
		if (!Events[i].status())
		// event was empty, will use it
		{
			break;
		}
	}

	// assign the new event to the empty event that was just found
	Events[i].activate(state, windowStart, windowEnd);
	return i;
}

void PushButtonArray::deleteEvent(uint8_t idx)
{
	Events[idx].deactivate();
}

eventOutput PushButtonArray::checkEvents(bool individualButtonStates[buttonNum])
{
	state = translateState(individualButtonStates);
	uint8_t stateToSend;
	bool released;

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

		// when releasing, interested in state array was
		stateToSend = lastState;
		released = true;
	}
	else
	{
		// updates the timer of the current state of the button array
		stateDuration[state] = millis() - stateStartTime[state];

		// when still in state, interested in current state
		stateToSend = state;
		released = false;
	}

	// Check if any events were triggered
	eventOutput output;					// prepare the output structure
	output.triggeredEvent = -1; // no events triggered by default

	// Serial.print("State to send: ");
	// Serial.print(stateToSend);
	// Serial.print(". State duration: ");
	// Serial.print(stateDuration[stateToSend]);
	// Serial.print(". Released: ");
	// Serial.println(released);

	for (int i = 0; i < 8; i++) // go through all the events
	{
		if (!Events[i].status())
		{ // event is not activate, skip it.
			continue;
		}
		output.triggeredEventType =
				Events[i].checkEvent(stateToSend, stateDuration[stateToSend], released);
		if (output.triggeredEventType != NONE) // a valid event happened
		{
			output.triggeredEvent = i;
			break; // so only the first detected event gets sent back
		}
	}

	lastState = state;
	return output;
}