#pragma once

#include <cstdint>

/**
 * @enum dueType
 * @brief Define how measurements can be due
 */
typedef enum
{
    NOT_DUE,     ///< Measurement not due
    DUE_BY_TIME, ///< Due by time
    DUE_BY_DR    ///< Due by Data Ready input
} dueType;

/**
 * @enum packetType
 * @brief Define different types of packets; use 1 byte
 */
enum packetType : uint8_t
{
    NO_PACKET,                     ///< No packet. indicates EOF
    ADIS16470_PACKET_ARCHIVE_TYPE, ///< ADIS16470 Archived Packet
    AISx120SX_PACKET_ARCHIVE_TYPE, ///< AISx120SX Archived Packet
    RSC_PRESSURE_PACKET_TYPE,      ///< Honewell RSC pressure packet
    RSC_TEMP_PACKET_TYPE,          ///< Honewell RSC temperature packet
    MAX31855_PACKET_ARCHIVE_TYPE,  ///< MAX31855 Archived packet
    ALTIMAX_PACKET_TYPE,           ///< ALTIMAX packet
    ADIS16470_PACKET_TYPE,         ///< ADIS16470 Packet
    AISx120SX_PACKET_TYPE,         ///< AISx120SX Packet
    MAX31855_PACKET_TYPE,          ///< MAX31855 packet
    MAX7_PACKET_TYPE,              ///< MAX7 packet
    XBee_PACKET_TYPE,              ///< XBee packet
    AD8556_PACKET_TYPE             ///< AD8556 packet
};

/**
 * @brief Convert from \p packetType to a printable string
 *
 * @param t : The packet
 * @return const char* : Its string representation
 */
const char *packetTypeStr(const packetType t);
