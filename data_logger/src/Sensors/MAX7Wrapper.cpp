#include "MAX7Wrapper.hpp"

// initialize the sensor count
uint8_t MAX7Wrapper::sensorQty = 0;

// constructor
MAX7Wrapper::MAX7Wrapper(uint8_t rx, uint8_t tx)
    : Sensor(sensorQty),
      lastPacket(getHeader(0)),
      Rx(rx),
      Tx(tx)
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

    SoftwareSerial mySerial(Rx, Tx);
    mySerial.begin(38400);

    // Try to see if the MAX7 is working
    for (uint32_t i = 0; i < attempts; i++)
    {

        if (!gnss.begin(mySerial, delayDuration))
        {
            gnss.enableDebugging(Serial);
            gnss.setFileBufferSize(sizeof(struct MAX7Body) * 100);

            gnss.setUART1Output(COM_TYPE_UBX); // Set the UART port to output UBX only
            gnss.setI2COutput(COM_TYPE_UBX);   // Set the I2C port to output UBX only (turn off NMEA noise)
            gnss.setAutoPVT(true, true, delayDuration);    // Ask for periodic updates

            if (gnss.saveConfiguration(delayDuration)) // Save the current settings to flash and BBR
            {
                active = true;
                return active;
            }
            else
            {
                if (SERIAL_PRINT)
                {
                    Serial.print("[MAX7 Setup] Config timeout\n");
                }
            }
        }
        else
        {
            if (SERIAL_PRINT)
            {
                Serial.print("[MAX7 Setup] Failed gnss.begin\n");
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
    return false;

    bool returnVal = false;
    if (isDueByTime(currMicros))
    {
        // read the measurements from the sensor
        uint32_t latitude, longitude, altitude;

        Serial.println("[MAX7] Entering id due by time");

        latitude = gnss.getLatitude(MEASUREMENT_MARGIN / 2);

        Serial.println("[MAX7] Got latitude");
        longitude = gnss.getLongitude(MEASUREMENT_MARGIN / 2);
        altitude = gnss.getAltitude(MEASUREMENT_MARGIN / 2);
        if (lastPacket.getLatitude() != latitude ||
            lastPacket.getLongitude() != longitude ||
            lastPacket.getAltitude() != altitude)
        {
            prevMeasTime = currMicros;
            returnVal = true;

            lastPacket.setLatitude(latitude);
            lastPacket.setLongitude(longitude);
            lastPacket.setAltitude(altitude);
        }
    }

    return returnVal;
}

bool MAX7Wrapper::isMeasurementInvalid()
{
    return true;
}

MAX7Packet *MAX7Wrapper::getPacket(uint32_t currMicros)
{
    // update the error on the packet
    lastPacket.updateHeader(getHeader(currMicros));

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