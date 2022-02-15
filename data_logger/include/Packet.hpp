#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>

#include <exception>
#include <string>

#include "CustomTypes.hpp"

const uint8_t ERROR_TYPE_NUM = 5; ///< Number of errors

/// 8 byte header for all packets
struct PacketHeader
{
    packetType packetType_ = NO_PACKET; ///< Type of the packet, 1 byte
    uint8_t packetSize = 0;             ///< Size in bytes of the packet, 1 byte
    uint8_t sensorID = 0;               ///< ID of the sensor which produced the packet, 1 byte
    uint8_t errorCode = 0xff;           ///< Error code, 1 byte; @see void decodeErrorCode(bool errorArray[ERROR_TYPE_NUM], uint8_t errorCode)
    uint32_t timestamp = 0;             ///< Timestamp, in microseconds, 4 bytes
};

#define PACKET_HEADER_FORMAT "**************** PACKET HEADER *****************\n" \
                             "Packet type: %s\n"                                  \
                             "Packet size: %4d\n"                                 \
                             "Sensor ID:   %4d\n"                                 \
                             "Errors:\n"                                          \
                             "\t- Measurement is late:        %2d\n"              \
                             "\t- Several check beat skipped: %2d\n"              \
                             "\t- DR pin didn't trigger read: %2d\n"              \
                             "\t- Checksum error:             %2d\n"              \
                             "\t- Measurement invalid:        %2d\n"              \
                             "Timestamp:   %12d\n"                                \
                             "**************** END OF HEADER *****************\n"

/**
 * @brief A line of Packet header is at most 47, and 11 lines
 */
#define PACKET_HEADER_PRINT_SIZE 48 * 11

class Packet
{
public:
    // ----- Constructors ----- //

    /**
     * @brief Construct a new empty Packet object
     *
     * The header is default (special \c packetType_ , all values to 0, all error flags raised)
     * The data is NULL
     */
    Packet();

    /**
     * @brief Construct a new Packet object and allocate memory for data
     *
     * The header is default, except for the size (which must be positive)
     *
     * @param size Size in bytes of the expected content
     */
    Packet(uint8_t size);

    /**
     * @brief Construct a new Packet object from a given header
     *
     * Set the header to the one provided and allocate memory according the the size
     * indicated in the header
     *
     * @param header Header for the packet, must have positive size
     */
    Packet(PacketHeader &header);

    /**
     * @brief Copy constructor
     *
     * @param other
     */
    Packet(Packet &other);

    // ----- Destructor ----- //
    /**
     * @brief Destroy the Packet object
     *
     * Free Packet::content if not already to \c NULL, and set it to \c NULL
     */
    ~Packet();

    // ----- Setters ----- //
    /**
     * @brief Set the packet's content
     *
     * @param data Source pointer for data
     * @param s Number of bytes to copy
     * @return uint8_t : Number of bytes actually copied (min between \p s and Packet::header.packetSize )
     */
    uint8_t setContent(void *data, size_t s);

    /**
     * @brief Set the packet Type in the header
     *
     * @param pt
     */
    void setPacketType(packetType pt)
    {
        header.packetType_ = pt;
    }

    /**
     * @brief Set the sensor ID in the header
     *
     * @param id
     */
    void setSensorID(uint8_t id)
    {
        header.sensorID = id;
    }

    /**
     * @brief Set the error code in the header
     *
     * @param errCode
     */
    void setErrorCode(uint8_t errCode)
    {
        header.errorCode = errCode;
    }

    /**
     * @brief Set the timestamp in the header
     *
     * @param ts
     */
    void setTimestamp(uint32_t ts)
    {
        header.timestamp = ts;
    }

    /**
     * @brief Update the header with the one given
     *
     * The packet's size and type have to be the same in the current header and the one provided.
     *
     * @param h
     */
    void updateHeader(PacketHeader h);

    // ----- Getters ----- //

    /**
     * @brief Get the packet type from header
     *
     * @return packetType
     */
    packetType getPacketType()
    {
        return header.packetType_;
    }

    /**
     * @brief Get the packet size from header
     *
     * @return uint8_t
     */
    uint8_t getPacketSize()
    {
        return header.packetSize;
    }

    /**
     * @brief Get the sensor ID from header
     *
     * @return uint8_t
     */
    uint8_t getSensorID()
    {
        return header.sensorID;
    }

    /**
     * @brief Get the error code from header
     *
     * @return uint8_t
     */
    uint8_t getErrorCode()
    {
        return header.errorCode;
    }

    /**
     * @brief Get the timestamp from header
     *
     * @return uint8_t
     */
    uint8_t getTimestamp()
    {
        return header.timestamp;
    }

    /**
     * @brief Copy Packet::content to holder
     *
     * @param holder A buffer of size as least packetSize
     */
    void getContent(void *holder)
    {
        memcpy(holder, content, header.packetSize);
    }

    // ----- Accessors ----- //

    /**
     * @brief Get a pointer to the packet's header
     *
     * @return PacketHeader*
     */
    PacketHeader *accessHeader()
    {
        return &header;
    }

    /**
     * @brief Get a copy of the data pointer
     *
     * @return void*
     */
    void *accessContent()
    {
        return content;
    }

    /**
     * @brief Return a pointer toward a printable description of the header
     *
     * @return char* : Pointer toward formated header description
     */
    char *getPrintableHeader()
    {
        char output[PACKET_HEADER_PRINT_SIZE];

        bool error_array[ERROR_TYPE_NUM] = {false};

        decodeErrorCode(error_array, header.errorCode);

        snprintf(output, PACKET_HEADER_PRINT_SIZE, PACKET_HEADER_FORMAT,
                 packetTypeStr(header.packetType_),
                 header.packetSize,
                 header.sensorID,
                 error_array[0],
                 error_array[1],
                 error_array[2],
                 error_array[3],
                 error_array[4],
                 header.timestamp);

        return output;
    }

    /**
     * @brief Return a pointer toward a printable description of the content
     *
     * @return char* : Pointer toward formated content description
     */
    virtual char *getPrintableContent() = 0;

protected:
    PacketHeader header;
    void *content = NULL;
};
