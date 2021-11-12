#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>

#include <exception>
#include <string>

#include "CustomTypes.hpp"

const uint8_t ERROR_TYPE_NUM = 5; // type of errors available

// 8 byte header for all packets
struct PacketHeader
{
    packetType packetType_ = NO_PACKET; // 1 byte
    uint8_t packetSize = 0;             // 1 byte
    uint8_t sensorID = 0;               // 1 byte
    uint8_t errorCode = 0xff;           // 1 byte
    uint32_t timestamp = 0;             // 4 bytes
};

class Packet
{
public:
    // ----- Constructors ----- //
    Packet();
    Packet(uint8_t size);
    Packet(PacketHeader header);
    Packet(Packet &other);

    // ----- Destructor ----- //
    ~Packet();

    // ----- Setters ----- //
    uint8_t setContent(void *data, size_t s);

    void setPacketType(packetType pt)
    {
        header.packetType_ = pt;
    }

    void setSensorID(uint8_t id)
    {
        header.sensorID = id;
    }

    void setErrorCode(uint8_t errCode)
    {
        header.errorCode = errCode;
    }

    void setTimestamp(uint32_t ts)
    {
        header.timestamp = ts;
    }

    void updateHeader(PacketHeader h);

    // ----- Getters ----- //

    packetType getPacketType()
    {
        return header.packetType_;
    }

    uint8_t getPacketSize()
    {
        return header.packetSize;
    }

    uint8_t getSensorID()
    {
        return header.sensorID;
    }

    uint8_t getErrorCode()
    {
        return header.errorCode;
    }

    uint8_t getTimestamp()
    {
        return header.timestamp;
    }

    // Holder should be at least as long as packetSize
    void getContent(void *holder)
    {
        memcpy(holder, content, header.packetSize);
    }

    // ----- Accessors ----- //

    PacketHeader* accessHeader()
    {
        return &header;
    }

    void* accessContent()
    {
        return content;
    }

protected:
    PacketHeader header;
    void *content = NULL;
};
