#pragma once

#include <cstdint>

/**
 * @brief Define how measurements can be due
 * 
 */
typedef enum
{
    NOT_DUE,     ///< Measurement not due
    DUE_BY_TIME, ///< Due by time
    DUE_BY_DR    ///< Due by Data Ready input
} dueType;

/**
 * @brief Define different types of packets; use 1 byte
 */
enum packetType : uint8_t
{
    NO_PACKET,                     ///< no packet. indicates EOF
    ADIS16470_PACKET_ARCHIVE_TYPE, ///< ADIS16470 Archived Packet
    AISx120SX_PACKET_ARCHIVE_TYPE, ///< AISx120SX Archived Packet
    RSC_PRESSURE_PACKET_TYPE,      ///< Honewell RSC pressure packet
    RSC_TEMP_PACKET_TYPE,          ///< Honewell RSC temperature packet
    MAX31855_PACKET_ARCHIVE_TYPE,  ///< MAX31855 Archived packet
    ALTIMAX_PACKET_TYPE,           ///< ALTIMAX packet
    ADIS16470_PACKET_TYPE,         ///< ADIS16470 Packet
    AISx120SX_PACKET_TYPE,         ///< AISx120SX Packet
    MAX31855_PACKET_TYPE           ///< MAX31855 packet
};

/**
 * @brief Convert from \p packetType to a printable string 
 * 
 * @param t : The packet
 * @return const char* : Its string representation
 */
const char *packetTypeStr(const packetType t)
{
    switch (t)
    {
    case NO_PACKET:
        return "NO_PACKET";
        break;

    case ADIS16470_PACKET_ARCHIVE_TYPE:
        return "ADIS16470_PACKET_ARCHIVE_TYPE";
        break;

    case AISx120SX_PACKET_ARCHIVE_TYPE:
        return "AISx120SX_PACKET_ARCHIVE_TYPE";
        break;

    case RSC_PRESSURE_PACKET_TYPE:
        return "RSC_PRESSURE_PACKET_TYPE";
        break;

    case RSC_TEMP_PACKET_TYPE:
        return "RSC_TEMP_PACKET_TYPE";
        break;

    case MAX31855_PACKET_ARCHIVE_TYPE:
        return "MAX31855_PACKET_ARCHIVE_TYPE";
        break;

    case ALTIMAX_PACKET_TYPE:
        return "ALTIMAX_PACKET_TYPE";
        break;

    case ADIS16470_PACKET_TYPE:
        return "ADIS16470_PACKET_TYPE";
        break;

    case AISx120SX_PACKET_TYPE:
        return "AISx120SX_PACKET_TYPE";
        break;

    case MAX31855_PACKET_TYPE:
        return "MAX31855_PACKET_TYPE";
        break;

    default:
        return "-- unknown packet type --";
        break;
    }
}
