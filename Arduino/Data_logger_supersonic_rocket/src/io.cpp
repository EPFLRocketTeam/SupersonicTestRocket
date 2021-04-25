/*
 * io.cpp
 *
 *  Created on: 2021-04-24
 *      Author: Joshua Cayetano-Emond
 */

#include "io.h"

void flashLED(uint8_t pinNumber, int flashDuration)
{
  digitalWrite(pinNumber, HIGH);
  delay(flashDuration);
  digitalWrite(pinNumber, LOW);
  //delay(flashDuration);
}

void successFlash()
{
  flashLED(GREEN_LED_PIN, SUCCESS_FLASH_DURATION);
}

void errorFlash()
{
  flashLED(RED_LED_PIN, ERROR_FLASH_DURATION);
}