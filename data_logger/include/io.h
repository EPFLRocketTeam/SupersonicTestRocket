/*
 * io.h
 *
 *  Created on: 2021-04-24
 *      Author: Joshua Cayetano-Emond
 */
#pragma once
#include <Arduino.h>

#include "globalVariables.h"

const int SUCCESS_FLASH_DURATION = 500;
const int ERROR_FLASH_DURATION = 1500;

// flashes the given LED for a certain time
// \!/ uses delay(). do not use in any time-sensitive application
void flashLED(uint8_t pinNumber, int flashDuration);

// flash the LED when something was successful
// \!/ uses delay(). do not use in any time-sensitive application
void successFlash();
// flash the LED when something had an error
// \!/ uses delay(). do not use in any time-sensitive application
void errorFlash();