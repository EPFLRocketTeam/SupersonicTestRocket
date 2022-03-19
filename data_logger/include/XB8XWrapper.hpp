#pragma once

// Standard libraries
#include <Arduino.h>
#include <SoftwareSerial.h>

// User-defined headers
#include "Packet.hpp"
#include "macrofunctions.h"
#include "globalVariables.hpp"

class XB8XWrapper
{
private:
    SoftwareSerial serial;
    uint32_t destH,destL; ///< High and Low parts of the destination's address (XBee default is Broadcast)
    uint8_t API_mode = 0; ///< API mode, 0 is transparent mode (no API, default), 1 is API without Escapes and 2 is API with Escapes

    /**
     * @brief Used to enter the XBee command mode
     *
     * @warning This is slow ! It needs at least 2s to setup
     * 
     * @param waitAns How many milliseconds to wait for answer; default is 1000 ms
     * @return true : successfully entered command mode
     * @return false : failed to enter command mode
     */
    bool enterCmdMode(uint32_t waitAns = 1000);

    /**
     * @brief Exit command mode 
     */
    void exitCmdMode();

public:
    /**
     * @brief Construct a new XB8XWrapper object
     *
     * @param Rx Index for Rx pin
     * @param Tx Index for Tx pin
     */
    XB8XWrapper(uint8_t Rx, uint8_t Tx);

    /**
     * @brief Set the destination in XBee (store it and update it)
     * 
     * @warning Needs to use Commad mode, which is slow. @see enterCmdMode()
     * 
     * @param destH 
     * @param destL 
     */
    void setDestination(uint32_t destH, uint32_t destL);

};