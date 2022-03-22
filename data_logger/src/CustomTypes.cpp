#include "CustomTypes.hpp"

const char *packetTypeStr(const packetType t)
{
    switch (t)
    {
    case NO_PACKET:
        return "NO_PACKET";
        break;

    case ADIS16470_PACKET_ARCHIVE_TYPE:
        return "ADIS16470_ARCHIVE";
        break;

    case AISx120SX_PACKET_ARCHIVE_TYPE:
        return "AISx120SX_ARCHIVE";
        break;

    case RSC_PRESSURE_PACKET_TYPE:
        return "RSC_PRESSURE";
        break;

    case RSC_TEMP_PACKET_TYPE:
        return "RSC_TEMP";
        break;

    case MAX31855_PACKET_ARCHIVE_TYPE:
        return "MAX31855_ARCHIVE";
        break;

    case ALTIMAX_PACKET_TYPE:
        return "ALTIMAX";
        break;

    case ADIS16470_PACKET_TYPE:
        return "ADIS16470";
        break;

    case AISx120SX_PACKET_TYPE:
        return "AISx120SX";
        break;

    case MAX31855_PACKET_TYPE:
        return "MAX31855";
        break;

    default:
        return "-- unknown packet type --";
        break;
    }
}