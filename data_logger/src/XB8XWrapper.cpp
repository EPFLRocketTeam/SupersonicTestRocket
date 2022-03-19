#include "XB8XWrapper.hpp"

XB8XWrapper::XB8XWrapper(uint8_t Rx, uint8_t Tx)
    : serial(Rx, Tx)
{
    serial.begin(9600);
}

bool XB8XWrapper::enterCmdMode(uint32_t waitAns)
{
    delay(1000);
    serial.write("+++");
    delay(1000);

    int ans[3];
    size_t i = 0;
    uint32_t startTime = millis();

    while (millis() - startTime < waitAns)
    {
        if (serial.available())
        {
            ans[i] = serial.read();
            i++;
        }

        // XBee should return 'OK\r' upon successfully entering commande mode
        if (i >= 3)
            return ans[0] == 'O' && ans[1] == 'K' && ans[2] == '\r';
    }

    return 0;
}

void XB8XWrapper::exitCmdMode()
{
    serial.write("CN\r");
}

void XB8XWrapper::setDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    char destHBuffer[11] = "";
    char destLBuffer[11] = "";

    snprintf(destHBuffer,10,"%8.8lX",destH);
    snprintf(destLBuffer,10,"%8.8lX",destL);

    serial.write("ATDH"); // Set high 32-bits of destination address
    serial.write(destHBuffer);
    serial.write('\r');
    
    serial.write("ATDL"); // Set low 32-bits of destination address
    serial.write(destLBuffer);
    serial.write('\r');
}

void XB8XWrapper::changeDestination(uint32_t dH, uint32_t dL)
{
    destH = dH;
    destL = dL;

    char destHBuffer[11] = "";
    char destLBuffer[11] = "";

    snprintf(destHBuffer,10,"%8.8lX",destH);
    snprintf(destLBuffer,10,"%8.8lX",destL);

    enterCmdMode();
    serial.write("ATDH"); // Set high 32-bits of destination address
    serial.write(destHBuffer);
    serial.write('\r');
    
    serial.write("ATDL"); // Set low 32-bits of destination address
    serial.write(destLBuffer);
    serial.write('\r');
    exitCmdMode();
}