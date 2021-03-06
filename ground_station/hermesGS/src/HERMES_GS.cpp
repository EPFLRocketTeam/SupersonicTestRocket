/*!
 * \file HERMES_GS.cpp
 *
 * \brief Hermes Ground Support Equipment Transceiver
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
    std::cout << "Syntax : [Tx] ./hermesGS [serial port] [DatagramID]" << std::endl;
    std::cout << "         [Rx] ./hermesGS [serial port]" << std::endl;
    std::cout << "Syntax ex [Tx] : ./hermesGS ttyS3 27" << std::endl;

    using namespace DatagramType;

    signal(SIGINT, sig_handler);

    std::string port("/dev/ttyUSB0");
    bool modeTx(false);
    DatagramID ID(INIT);

    switch (argc) {
        case 3:
            port = "/dev/" + std::string(argv[1]);
            ID = (DatagramID) atoi(argv[2]);
            modeTx = 1;             // Tx
            break;
        case 2:
            port = "/dev/" + std::string(argv[1]);
            modeTx = 0;             // Rx
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

    while (keep_running) {
            // Transmitter
        if (modeTx) {
            dataHandler.updateTx(ID);
            xbee.send(dataHandler.getPacket(ID));

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
            // Receiver
        else {
            if (xbee.receive(dataHandler)) {
                dataHandler.printLastRxPacket();
                dataHandler.logLastRxPacket();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    return 0;
}