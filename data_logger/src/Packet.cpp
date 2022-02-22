#include "Packet.hpp"

// ----- Constructors ----- //

Packet::Packet()
{
    header = {};
    content = NULL;
}

Packet::Packet(uint8_t size)
{
    assert(size != 0);
    header = {};
    header.packetSize = size;
    content = malloc(size);
}

Packet::Packet(PacketHeader &h)
{
    assert(h.packetSize == 0);
    header = h;
    content = malloc(h.packetSize);
}

Packet::Packet(Packet &other)
{
    header = other.header;
    content = malloc(header.packetSize);
    memcpy(content, other.content, other.header.packetSize);
}

// ----- Destructor ----- //

Packet::~Packet()
{
    if (content != NULL)
    {
        free(content);
        content = NULL;
    }
}

// ----- Setters ----- //

uint8_t Packet::setContent(void *data, size_t s)
{
    // Number of bytes copied is at most header.packetSize
    uint8_t copied = (s > header.packetSize) ? header.packetSize : s;
    memcpy(content, data, copied);
    return copied;
}

void Packet::updateHeader(PacketHeader h)
{
    assert(h.packetSize == header.packetSize &&
           h.packetType_ == header.packetType_);

    header = h;
}

void Packet::getPrintableHeader(char *buff)
    {
        // char output[PACKET_HEADER_PRINT_SIZE];

        bool error_array[ERROR_TYPE_NUM] = {false};

        decodeErrorCode(error_array, header.errorCode);

        snprintf(buff, PACKET_HEADER_PRINT_SIZE, PACKET_HEADER_FORMAT,
                 packetTypeStr(header.packetType_),
                 header.packetSize,
                 header.sensorID,
                 error_array[0],
                 error_array[1],
                 error_array[2],
                 error_array[3],
                 error_array[4],
                 header.timestamp);
    }

uint8_t getErrorCode(bool *errorArray)
{
  uint8_t errorCode = 0;

  for (size_t i = 0; i < ERROR_TYPE_NUM; i++)
  {
    if (errorArray[i])
    {
      bitSet(errorCode, 7 - i);
    }
  }

  return errorCode;
}

// takes an error code and transforms it into a boolean array
void decodeErrorCode(bool errorArray[ERROR_TYPE_NUM], uint8_t errorCode)
{
  for (size_t i = 0; i < ERROR_TYPE_NUM; i++)
  {
    errorArray[i] = bitRead(errorCode, 7 - i);
  }
}