#pragma once

// Standard libraries
#include <Arduino.h>
#include <SoftwareSerial.h>

// User-defined headers
#include "Packet.hpp"
#include "macrofunctions.h"
#include "globalVariables.hpp"

// Fetch datagram IDs from GS part
#include <DatagramTypes.h>

/**
 * @brief Generic XBee API frame structure
 *
 * See at : https://www.digi.com/resources/documentation/digidocs/pdfs/90001538.pdf#page=145
 *
 */
struct XBeeAPI_Frame
{
    const uint8_t start_delimiter = 0x7E;
    uint8_t length_MB; // Most significant byte
    uint8_t length_LB; // Least significant byte
    void *frame_data;
    uint8_t checksum;
};

/**
 * @brief XBee API transmit requestion frame structure
 *
 * See at : https://www.digi.com/resources/documentation/digidocs/pdfs/90001538.pdf#page=155
 *
 */
struct XBeeAPI_Transmit_Request_Frame
{
    const uint8_t frame_type = 0x10;
    uint8_t frame_id = 0;                  // If 0, the device will not emit a response frame
    uint64_t destination_address = 0xFFFF; // 64-bit IEEE address of destination; broadcast address if 0xFFFF
    const uint16_t reserved = 0xFFFE;
    uint8_t broadcast_radius = 0;   // Maximal number of hops a broadcast can traverse. If set to 0, use the value of NH (XBee internal parameter)
    uint8_t transmit_option = 0x43; // 0x43 : disable ACK and Route discovery. If set to 0, use the value of TO (XBee internal parameter)
    void *payload;                  // Data to send to the destination device. Use up to NP bytes per packet
};

/**
 * @brief EPFL XBee payload format
 *
 */
struct XBeeAPI_EPFL_Packet
{
    uint8_t datagram_ID;
    const uint8_t epfl_prefix[4] = {'E', 'P', 'F', 'L'};
    uint32_t timestamp; // In ms
    void *data;         // Data goes here, organised according to datagram_ID
};

/**
 * @brief Full, flattened frame (with all required metadata)
 *
 */
struct XBeeAPI_EPFL_Transmit_Frame
{
    const uint8_t start_delimiter = 0x7E;
    uint16_t length; // Length (everything except start_delimiter, length and checksum)
    const uint8_t frame_type = 0x10;
    uint8_t frame_id = 0;                  // If 0, the device will not emit a response frame
    uint64_t destination_address = 0xFFFF; // 64-bit IEEE address of destination; broadcast address if 0xFFFF
    const uint16_t reserved = 0xFFFE;
    uint8_t broadcast_radius = 0;   // Maximal number of hops a broadcast can traverse. If set to 0, use the value of NH (XBee internal parameter)
    uint8_t transmit_option = 0x43; // 0x43 : disable ACK and Route discovery. If set to 0, use the value of TO (XBee internal parameter)
    uint8_t datagram_ID;
    const uint8_t epfl_prefix[4] = {'E', 'P', 'F', 'L'};
    uint32_t timestamp; // In ms
    void *data;         // Data goes here, organised according to datagram_ID
    uint8_t checksum;
};

class XB8XWrapper
{
private:
    Stream* serial;
    uint32_t destH, destL; ///< High and Low parts of the destination's address (XBee default is Broadcast)
    uint8_t API_mode = 0;  ///< API mode, 0 is transparent mode (no API, default), 1 is API without Escapes and 2 is API with Escapes

    static const uint16_t XBee_Max_FrameSize = 255;
    uint8_t sendBuffer[XBee_Max_FrameSize]; ///< Buffered frame

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

    /**
     * @brief Set the destination in XBee (store it and update it)
     *
     * @warning Needs to be Commad mode
     *
     * @param destH High 32-bits of the receiver MAC address
     * @param destL Low 32-bits of the receiver MAC address
     */
    void setDestination(uint32_t destH, uint32_t destL);

    void setSendBufferHead();

public:
    /**
     * @brief Construct a new XB8XWrapper object
     *
     * @param serial Pointer toward the associated serial
     *
     * By default, the receiver address is 0x000000000000FFFF (Broadcast address)
     *
     * @param destH High 32-bits of the receiver MAC address
     * @param destL Low 32-bits of the receiver MAC address
     *
     */
    XB8XWrapper(Stream* serial, uint32_t destH = 0, uint32_t destL = 0xFFFF);

    /**
     * @brief Change the destination in XBee (store it and update it)
     *
     * @warning Needs to use Commad mode, which is slow. @see enterCmdMode()
     *
     * @param destH High 32-bits of the receiver MAC address
     * @param destL Low 32-bits of the receiver MAC address
     */
    void changeDestination(uint32_t destH, uint32_t destL);

    /**
     * @brief  Implementation of setup for XB8XWrapper
     *
     * Setup the destination and usual mode of operation (API mode)
     *
     * @param attempts Number of allowed attempts to try setting up the sensor
     * @param delayDuration Delay between the tries
     * @return true : The Sensor is correctly set up
     * @return false : The Sensor failed to set up
     */
    bool setup(uint32_t attempts, uint32_t delayDuration);

    void send(Packet *pkt);
};
