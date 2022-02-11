/*
 * io.hpp
 *
 *  Created on: 2021-04-24
 *      Author: Joshua Cayetano-Emond
 */
#pragma once
#include <Arduino.h>

#include "globalVariables.hpp"

const int SUCCESS_FLASH_DURATION = 500;
const int ERROR_FLASH_DURATION = 1500;

/**
 * @brief Flashes the given LED for a certain time. 
 * 
 * @warning Uses delay(). Do not use in any time-sensitive application
 * 
 * @param pinNumber Pin number of the LED
 * @param flashDuration Duration in microseconds for the flash
 */
void flashLED(uint8_t pinNumber, int flashDuration);

/**
 * @brief Wrapper of flashLED for to signal success
 * 
 * @warning Uses delay()
 */
void successFlash();

/**
 * @brief Wrapper of flashLED for to signal error
 * 
 * @warning Uses delay()
 */
void errorFlash();