#include "Sensor.hpp"
#include <exception>
#include <string.h>

class Packet
{
    public:
        // Constructors
        Packet() = delete; // Only use allocated packets
        Packet(uint8_t size);
        Packet(PacketHeader header);
        Packet(Packet &other);

        // Destructor
        ~Packet();

        // Setter
        uint8_t setContent(void* data, size_t s);

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

        // Getters

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
        void getContent(void* holder)
        {
            memcpy(holder,content,header.packetSize);
        }

    private:

    PacketHeader header;
    void* content;

};

class EmptyPacket : public std::exception
{
    const char* what() const throw()
    {
        return "Cannot build packet of size 0";
    }
};