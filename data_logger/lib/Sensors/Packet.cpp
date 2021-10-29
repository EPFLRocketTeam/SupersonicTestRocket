#include "Packet.hpp"

// ---------- Constructors ---------- //

Packet::Packet(uint8_t size)
{
    if (size == 0)
    {
        throw EmptyPacket();
    }
    header = {0};
    header.packetSize = size;
    content = malloc(size);

}

Packet::Packet(PacketHeader h)
{
    if (h.packetSize == 0)
    {
        throw EmptyPacket();
    }
    header = h;
    content = malloc(h.packetSize);
}

Packet::Packet(Packet &other)
{
    header = other.header;
    memcpy(content,other.content,other.header.packetSize);
}

// ---------- Destructor ---------- //

Packet::~Packet()
{
    free(content);
}

// ---------- Setters ---------- //

uint8_t Packet::setContent(void* data, size_t s)
{
    // Number of bytes copied is at most header.packetSize
    uint8_t copied = (s > header.packetSize) ? header.packetSize : s;
    memcpy(content,data,copied);
    return copied;
}

