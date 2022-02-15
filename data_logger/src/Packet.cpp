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
