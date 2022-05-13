#include "MAX7Wrapper.hpp"

// initialize the sensor count
uint8_t MAX7Wrapper::sensorQty = 0;

// constructor
MAX7Wrapper::MAX7Wrapper(Stream *s)
    : Sensor(sensorQty),
      mySerial(s),
      lastPacket(getHeader(0))

{
    setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN,
                    MEASUREMENT_INTERVAL, false);
    sensorQty += 1;
    active = false;
}

// destructor
MAX7Wrapper::~MAX7Wrapper()
{
    sensorQty -= 1;
}

bool MAX7Wrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
    // Try to see if the MAX7 is working
    uint32_t timeStart = millis();
    while (millis() - timeStart < attempts * delayDuration * 10)
    {
        while (mySerial->available())
        {
            char c = mySerial->read();
            if (gps.encode(c))
            {
                // Complete NMEA command : module is OK
                active = true;
                return active;
            }
        }
    }
    active = false;
    return active; // setup was not succesful
}

uint8_t MAX7Wrapper::getSensorQty()
{
    return sensorQty;
}

bool MAX7Wrapper::isDue(uint32_t currMicros, unused(volatile bool &triggeredDR))
{
    bool returnVal = false;
    if (isDueByTime(currMicros))
    {
        //Serial.print("[MAX7] GNSS is due: ");
        while (mySerial->available())
        {
            char c = mySerial->read();
            //Serial.print(c);
            if (gps.encode(c) && gps.location.isValid())
            {
                prevMeasTime = currMicros;
                returnVal = true;
                lastPacket.setLatitude(gps.location.lat());
                lastPacket.setLongitude(gps.location.lng());
                lastPacket.setAltitude(gps.altitude.meters());

                // update the error on the packet
                lastPacket.updateHeader(getHeader(currMicros));
            }
        }
    }
    return returnVal;
}

bool MAX7Wrapper::isMeasurementInvalid()
{
    return true;
}

MAX7Packet *MAX7Wrapper::getPacket()
{
#ifdef DEBUG

    lastPacket.setLatitude(generateFakeData(-2000000, 2000000, micros(), 0, 8700000));
    lastPacket.setLongitude(generateFakeData(-2000000, 2000000, micros(), 0, 8700000));
    lastPacket.setAltitude(generateFakeData(0, 10000, micros(), 100 * SENSOR_ID, 100000000));

// when not debugging readings are updated in isDue()
#endif

    return &lastPacket;
}

PacketHeader MAX7Wrapper::getHeader(uint32_t currMicros)
{
    return Sensor::getHeader(MAX7_PACKET_TYPE,
                             sizeof(MAX7Body),
                             currMicros);
}