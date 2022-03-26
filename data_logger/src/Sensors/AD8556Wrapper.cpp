#include "AD8556Wrapper.hpp"

// initialize the sensor count
uint8_t AD8556Wrapper::sensorQty = 0;

// constructor
AD8556Wrapper::AD8556Wrapper(uint8_t digin, uint8_t vout,
                             uint8_t firstStageGain_, uint8_t secondStageGain_,
                             uint8_t offset_, uint8_t analogResolution_,
                             float minRead, float maxRead)
    : Sensor(sensorQty),
      opamp(digin),
      lastPacket(getHeader(0)),
      Digin(digin),
      Vout(vout),
      firstStageGain(firstStageGain_),
      secondStageGain(secondStageGain_),
      offset(offset_),
      analogResolution(analogResolution_),
      minReading(minRead),
      maxReading(maxRead),
      rescale((maxRead - minRead) / (2 << analogResolution_))
{
    setupProperties(CHECK_INTERVAL, MEASUREMENT_MARGIN,
                    MEASUREMENT_INTERVAL, false);
    sensorQty += 1;
    active = false;
}

// destructor
AD8556Wrapper::~AD8556Wrapper()
{
    sensorQty -= 1;
}

bool AD8556Wrapper::setup(uint32_t attempts, uint32_t delayDuration)
{
    active = true;
    return active;
}

uint8_t AD8556Wrapper::getSensorQty()
{
    return sensorQty;
}

bool AD8556Wrapper::isDue(uint32_t currMicros, unused(volatile bool &triggeredDR))
{
    bool returnVal = false;
    if (isDueByTime(currMicros))
    {
        // read the measurements from the sensor
        float rawReading = static_cast<float>(analogRead(Vout));
        lastPacket.setReading(minReading + rawReading * rescale);
        return true;
    }
    return returnVal;
}

bool AD8556Wrapper::isMeasurementInvalid()
{
    return true;
}

AD8556Packet *AD8556Wrapper::getPacket(uint32_t currMicros)
{
    // update the error on the packet
    lastPacket.updateHeader(getHeader(currMicros));

#ifdef DEBUG

    lastPacket.setReading(generateFakeData(0, 20000, micros(), 0, 9300000));

// when not debugging readings are updated in isDue()
#endif

    return &lastPacket;
}

PacketHeader AD8556Wrapper::getHeader(uint32_t currMicros)
{
    return Sensor::getHeader(AD8556_PACKET_TYPE,
                             sizeof(AD8556Body),
                             currMicros);
}