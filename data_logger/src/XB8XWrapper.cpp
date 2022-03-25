#include "XB8XWrapper.hpp"

XB8XWrapper::XB8XWrapper(Stream* s, uint32_t dH, uint32_t dL)
    : serial(s),
      destH(dH),
      destL(dL)
{}

bool XB8XWrapper::enterCmdMode(uint32_t waitAns)
{
    delay(1000);
    serial->write("+++");
    delay(1000);

    int ans[3];
    size_t i = 0;
    uint32_t startTime = millis();

    while (millis() - startTime < waitAns)
    {
        if (serial->available())
        {
            ans[i] = serial->read();
            i++;
        }

        // XBee should return 'OK\r' upon successfully entering commande mode
        if (i >= 3)
            return ans[0] == 'O' && ans[1] == 'K' && ans[2] == '\r';
    }

    return 0;
}

void XB8XWrapper::exitCmdMode()
{
    serial->write("CN\r");
}

void XB8XWrapper::setDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    char destHBuffer[11] = "";
    char destLBuffer[11] = "";

    snprintf(destHBuffer, 10, "%8.8lX", destH);
    snprintf(destLBuffer, 10, "%8.8lX", destL);

    serial->write("ATDH"); // Set high 32-bits of destination address
    serial->write(destHBuffer);
    serial->write('\r');

    serial->write("ATDL"); // Set low 32-bits of destination address
    serial->write(destLBuffer);
    serial->write('\r');
}

void XB8XWrapper::changeDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    char destHBuffer[11] = "";
    char destLBuffer[11] = "";

    snprintf(destHBuffer, 10, "%8.8lX", destH);
    snprintf(destLBuffer, 10, "%8.8lX", destL);

    enterCmdMode();
    serial->write("ATDH"); // Set high 32-bits of destination address
    serial->write(destHBuffer);
    serial->write('\r');

    serial->write("ATDL"); // Set low 32-bits of destination address
    serial->write(destLBuffer);
    serial->write('\r');
    exitCmdMode();
}

bool XB8XWrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
    for (uint32_t i = 0; i < attempts; i++)
    {
        if (!enterCmdMode()) // Enter the command mode
        {
            Serial.write("[XBee] Could not get in CmdMode\n");
            delay(delayDuration);
        }
        else
        {
            setDestination(destH, destL); // Set the wanted destination

            serial->write("ATAP 1\r"); // Set standard API mode

            exitCmdMode();
            return true;
        }
    }

    return false;
}

void XB8XWrapper::send(Packet *pkt)
{
    // ----- Computing packet variables ----- //

    // Length
    uint16_t length = sizeof(uint8_t)         // Frame type
                      + sizeof(uint8_t)       // Frame id
                      + sizeof(uint64_t)      // Destination address
                      + sizeof(uint16_t)      // Reserved field
                      + sizeof(uint8_t)       // Broadcast radius
                      + sizeof(uint8_t)       // Transmit option
                      + sizeof(uint8_t)       // Datagram ID
                      + sizeof(uint8_t) * 4   // EPFL prefix
                      + sizeof(uint32_t)      // Timestamp
                      + pkt->getPacketSize(); // Size of the 'data' field

    Serial.print("[XBee:send] Setting datatype\n");

    // Data type
    uint8_t datagram_ID;
    switch (pkt->getPacketType())
    {
    case RSC_PRESSURE_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_HoneywellRsc_Pressure;
        break;
    case RSC_TEMP_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_HoneywellRsc_Temp;
        break;
    case ALTIMAX_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_Altimax;
        break;
    case ADIS16470_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_ADIS16470;
        break;
    case AISx120SX_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_AISx120SX;
        break;
    case MAX31855_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_MAX31855;
        break;
    case MAX7_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_MAX7;
        break;
    default:
        // In this case, either we are sending special packets (yet to be defined)
        // or something happened
        // Or we want to test, in which case data should be plaintext ASCII
        datagram_ID = DatagramType::HERMES_TEST;
        break;
    }

    // Timestamp (HERMES timestamps are in microseconds, not milliseconds)
    uint32_t timestamp = pkt->getTimestamp();

    // ----- Setting send buffer ----- //

    Serial.print("[XBee:send] Setting send buffer\n");

    memset((void *)sendBuffer, 0, sizeof(sendBuffer));

    uint8_t *buffer_varptr = sendBuffer; // Used as argument for BIG_ENDIAN_WRITE

    // Start delimiter
    *(buffer_varptr++) = 0x7E;

    // Length
    BIG_ENDIAN_WRITE(length, buffer_varptr);

    // Frame type
    *(buffer_varptr++) = 0x10;

    // Frame ID
    *(buffer_varptr++) = 0;

    // Destination address
    BIG_ENDIAN_WRITE(destH, buffer_varptr);
    BIG_ENDIAN_WRITE(destL, buffer_varptr);

    // Reserved field
    *(buffer_varptr++) = 0xFF;
    *(buffer_varptr++) = 0xFE;

    // Broadcast radius
    *(buffer_varptr++) = 0;

    // Transmit option
    *(buffer_varptr++) = 0x43;

    // Datagram ID
    *(buffer_varptr++) = datagram_ID;

    // EPFL prefix
    *(buffer_varptr++) = 'E';
    *(buffer_varptr++) = 'P';
    *(buffer_varptr++) = 'F';
    *(buffer_varptr++) = 'L';

    // Timestamp
    BIG_ENDIAN_WRITE(timestamp, buffer_varptr);

    // Data
    pkt->getBigEndian(buffer_varptr);

    // Compute checksum
    for (size_t i = 0; i < length; i++)
    {
        *buffer_varptr += sendBuffer[i + 3]; // Go +3 because escape start delimiter and length
    }
    *buffer_varptr = 0xFF - *buffer_varptr;

    // Length of content + 1 byte of start delimiter
    //  + 2 bytes of length + 1 byte of checksum
    serial->write(sendBuffer, length + 4);
}