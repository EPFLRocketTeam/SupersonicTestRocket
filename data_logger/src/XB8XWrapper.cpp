#include "XB8XWrapper.hpp"

XB8XWrapper::XB8XWrapper(Stream *s, uint32_t dH, uint32_t dL)
    : serial(s),
      destH(dH),
      destL(dL)
{
}

bool XB8XWrapper::acknowledge(uint32_t waitAns)
{
    uint32_t startTime = millis();
    int ans[3];
    size_t i = 0;

    while (millis() - startTime < waitAns)
    {
        if (serial->available())
        {
            ans[i] = serial->read();
            delay(2);
            i++;
        }

        // XBee should return 'OK\r' upon successfully entering commande mode
        if (i >= 3)
            return ans[0] == 'O' && ans[1] == 'K' && ans[2] == '\r';
    }
    return false;
}

bool XB8XWrapper::enterCmdMode(uint32_t waitAns)
{
    delay(1000);
    serial->write("+++");
    delay(2000);

    return acknowledge(waitAns);
}

void XB8XWrapper::exitCmdMode(uint32_t waitAns)
{
    serial->write("ATWR\r");
    acknowledge(waitAns);
    delay(10);
    serial->write("ATCN\r");
    acknowledge(waitAns);
}

void XB8XWrapper::setDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    serial->write("ATDH"); // Set high 32-bits of destination address
    serial->write(dH);
    serial->write('\r');
    delay(10);
    acknowledge(1000);

    serial->write("ATDL"); // Set low 32-bits of destination address
    serial->write(dL);
    serial->write('\r');
    delay(10);
    acknowledge(1000);
}

void XB8XWrapper::changeDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    enterCmdMode();

    serial->write("ATDH "); // Set high 32-bits of destination address
    serial->write(dH);
    serial->write('\r');
    delay(10);
    acknowledge(1000);

    serial->write("ATDL "); // Set low 32-bits of destination address
    serial->write(dL);
    serial->write('\r');
    delay(10);
    acknowledge(1000);

    exitCmdMode();
}

bool XB8XWrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
    if (enterCmdMode())
    {
        serial->write("ATAP 1\r");
        delay(10);
        acknowledge(1000);
        delay(2000);

        exitCmdMode();
        delay(2000);
    }
    else
    {
        if (SERIAL_PRINT)
        {
            Serial.println("[XBee] Could not enter command mode");
        }
    }

    for (uint32_t i = 0; i < attempts; i++)
    {

        // Sending a test packet
        serial->write((uint8_t)0x7E);
        serial->write((uint8_t)0);
        serial->write((uint8_t)27);

        uint8_t buffer[27];
        buffer[0] = 0x10;
        buffer[1] = 1;

        buffer[2] = 0;
        buffer[3] = 0;
        buffer[4] = 0;
        buffer[5] = 0;

        buffer[6] = 0;
        buffer[7] = 0;
        buffer[8] = 0xFF;
        buffer[9] = 0xFF;

        buffer[10] = 0xFF;
        buffer[11] = 0xFE;

        buffer[12] = 0;
        buffer[13] = 0x42;

        buffer[14] = 27;

        buffer[15] = 'E';
        buffer[16] = 'P';
        buffer[17] = 'F';
        buffer[18] = 'L';

        buffer[19] = 0;
        buffer[20] = 0;
        buffer[21] = 0;
        buffer[22] = 42;

        buffer[23] = 2;
        buffer[24] = 2;
        buffer[25] = 2;
        buffer[26] = 2;

        uint8_t checksum = 0;
        for (size_t i = 0; i < sizeof(buffer); i++)
        {
            serial->write(buffer[i]);
            checksum += buffer[i];
        }
        checksum = 0xFF - checksum;
        serial->write(checksum);

        // Acknoledge a receive packet
        delay(500);
        uint32_t start_waiting = millis();
        uint8_t rcv_counter = 0;
        uint8_t rcv_buffer[11];

        while (rcv_counter < 11 && millis() - start_waiting < 500)
        {
            if (serial->available())
            {
                rcv_buffer[rcv_counter] = serial->read();
                delay(2);
                rcv_counter++;
            }
        }

        if (millis() - start_waiting > 500)
        {
            if (SERIAL_PRINT)
            {
                Serial.println("[XBee] Did not recevied transmit status");
            }
        }
        else
        {
            bool success = true;
            success &= (rcv_buffer[0] == 0x7E);
            success &= (rcv_buffer[1] == 0);
            success &= (rcv_buffer[2] == 7);
            success &= (rcv_buffer[3] == 0x8B);
            success &= (rcv_buffer[4] == 1);
            success &= (rcv_buffer[5] == 0xFF);
            success &= (rcv_buffer[6] == 0xFE);
            // success &= (rcv_buffer[7] == 1); // Number of data retransmissions
            success &= (rcv_buffer[8] == 0x00); // Delivery status, 0x00 for success
            // success &= (rcv_buffer[9] == 0x00); // Discovery status, 0x00 for no discovery ahead

            uint8_t rcv_buffer_checksum = 0;
            for (size_t a = 3; a < 10; a++)
            {
                rcv_buffer_checksum += rcv_buffer[a];
            }
            rcv_buffer_checksum = 0xFF - rcv_buffer_checksum;
            success &= (rcv_buffer[10] == rcv_buffer_checksum);

            if (success)
            {
                return true;
            }
            else
            {
                if (SERIAL_PRINT)
                {
                    Serial.println("[XBee] Feedback message verfication failed");
                    Serial.print("[XBee] Seen : ");
                    for (size_t b = 0; b < 11; b++)
                    {
                        Serial.printf("%2.2x ", rcv_buffer[b]);
                    }
                    Serial.print('\n');
                }
            }
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
    case AD8556_PACKET_TYPE:
        datagram_ID = DatagramType::HERMES_AD8556;
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

    //Serial.print("[XBee:send] Setting send buffer\n");

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

    
    serial->write(sendBuffer[0]);
    serial->write(sendBuffer[1]);
    serial->write(sendBuffer[2]);
    
    // Compute checksum
    // Go +3 because escape start delimiter and length
    for (size_t i = 0; i < length; i++)
        {
            serial->write(sendBuffer[i+3]);
            *buffer_varptr += sendBuffer[i+3];
        }

    for (size_t i = 0; i < length; i++)
    {
        *buffer_varptr += sendBuffer[i + 3]; 
    }
    *buffer_varptr = 0xFF - *buffer_varptr;

    // Length of content + 1 byte of start delimiter
    //  + 2 bytes of length + 1 byte of checksum
    serial->write(sendBuffer[length+3]);
}