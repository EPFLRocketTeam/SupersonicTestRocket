#include "AD8556Wrapper.hpp"

// initialize the sensor count
uint8_t AD8556Wrapper::sensorQty = 0;

// constructor
AD8556Wrapper::AD8556Wrapper(uint8_t digin, uint8_t vout,
                             uint8_t firstStageGain_, uint8_t secondStageGain_,
                             uint8_t offset_, uint8_t analogResolution_,
                             float minRead, float maxRead)
    : Sensor(sensorQty),
      lastPacket(getHeader(0)),
      Digin(digin),
      Vout(vout),
      firstStageGain(firstStageGain_),
      secondStageGain(secondStageGain_),
      offset(offset_),
      analogResolution(analogResolution_),
      minReading(minRead),
      maxReading(maxRead)
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
    // Try to see if the AD8556 is working
    for (uint32_t i = 0; i < attempts; i++)
    {
        // set Second Stage Gain code
        if (!opamp.setSecondStageGain(secondStageGain))
        {
            // opamp will return false if code is out of range
            Serial.println("Invalid Second Stage Gain code. Valid range is 0..7");
            return false;
        }
        // First Stage Gain code
        if (!opamp.setFirstStageGain(firstStageGain))
        {
            Serial.println("Invalid First Stage Gain code. Valid range is 0..127");
            return false;
        }

        // Offset code
        if (!opamp.setOffset(offset))
        {
            Serial.println("Invalid Offset code. Valid range is 0..255");
            return false;
        }
        // if (!gnss.begin(mySerial))
        // {
        //     gnss.setUART1Output(COM_TYPE_UBX); // Set the UART port to output UBX only
        //     gnss.setI2COutput(COM_TYPE_UBX);   // Set the I2C port to output UBX only (turn off NMEA noise)
        //     gnss.setAutoPVT(true);             // Ask for periodic updates
        //     gnss.saveConfiguration();          // Save the current settings to flash and BBR
        //     active = true;
        //     return active;
        // }
        // else // give it time before the next try
        // {
        //     delay(delayDuration);
        // }
    }
    active = false;
    return active; // setup was not succesful
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
        uint16_t rawReading = analogRead(Vout);
        lastPacket.setReading(rawReading * 1.);
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