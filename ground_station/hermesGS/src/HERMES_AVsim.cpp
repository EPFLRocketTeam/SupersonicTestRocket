/*!
 * \file HERMES_AVsim.cpp
 *
 * \brief Hermes Avionics Simulator
 *
 * \author      HA Michael - EPFL EL BA6
 * \date        23.03.2022
 */

#include <chrono>
#include <thread>

#include <Xbee.h>
#include <DataHandler.h>
#include <connector.h>
#include <csignal>

static volatile sig_atomic_t keep_running = 1;

static void sig_handler(int _) {
    (void)_;
    keep_running = 0;
}

int main(int argc, char** argv) {
    std::cout << "Syntax : [Tx] ./hermesGS [serial port]" << std::endl;
    std::cout << "Syntax ex [Tx] : ./hermesGS ttyS3" << std::endl;

    using namespace DatagramType;

    signal(SIGINT, sig_handler);

    std::string port("/dev/ttyUSB0");

    switch (argc) {
        case 2:
            port = "/dev/" + std::string(argv[1]);
            break;

        default:
            std::cout << "Bad syntax" << std::endl;
            return 0;
    }

    Connector connector;
    std::shared_ptr<Connector> cptr(&connector);

    // RF modem
    Xbee xbee(port);
    // RF packet handler
    DataHandler dataHandler(cptr);
    using namespace DatagramType;

    // send a single test packet at startup
    dataHandler.updateTx(HERMES_TEST);
    xbee.send(dataHandler.getPacket(HERMES_TEST));

    static const unsigned int delay(50);    // 20 packets/sec
    static unsigned int packetCount(1);
    static bool honeywell_toggle(0);
    while (keep_running) {
        dataHandler.updateTx(HERMES_ADIS16470);
        xbee.send(dataHandler.getPacket(HERMES_ADIS16470));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        dataHandler.updateTx(HERMES_AISx120SX);
        xbee.send(dataHandler.getPacket(HERMES_AISx120SX));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        dataHandler.updateTx(HERMES_Altimax);
        xbee.send(dataHandler.getPacket(HERMES_Altimax));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(honeywell_toggle==0) {
            honeywell_toggle = 1;
            dataHandler.updateTx(HERMES_HoneywellRsc_Pressure);
            xbee.send(dataHandler.getPacket(HERMES_HoneywellRsc_Pressure));
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        } else {
            honeywell_toggle = 0;
            dataHandler.updateTx(HERMES_HoneywellRsc_Temp);
            xbee.send(dataHandler.getPacket(HERMES_HoneywellRsc_Temp));
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        dataHandler.updateTx(HERMES_MAX7);
        xbee.send(dataHandler.getPacket(HERMES_MAX7));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        dataHandler.updateTx(HERMES_MAX31855);
        xbee.send(dataHandler.getPacket(HERMES_MAX31855));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        dataHandler.updateTx(HERMES_AD8556);
        xbee.send(dataHandler.getPacket(HERMES_AD8556));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));

        packetCount += 7;
        std::cout << "Packets sent : " << packetCount << std::endl;
    }
    return 0;
}