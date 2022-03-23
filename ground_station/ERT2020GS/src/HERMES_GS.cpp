//
// Created by Michael Ha on 22.03.22.
//

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
    std::cout << "Syntax : [Tx] ./hermesGS [DatagramID] [serial port]" << std::endl;
    std::cout << "         [Rx] ./hermesGS [serial port]" << std::endl;
    std::cout << "Syntax ex [Tx] : ./hermesGS 3 ttyS3" << std::endl;

    using namespace DatagramType;

    signal(SIGINT, sig_handler);

    std::string port("/dev/ttyUSB0");
    bool modeTx(false);
    DatagramID ID(INIT);

    switch (argc) {
        case 3:
            port = "/dev/" + std::string(argv[2]);
            ID = (DatagramID) atoi(argv[1]);
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

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
            // Receiver
        else {
            if (xbee.receive(dataHandler)) {
                dataHandler.printLastRxPacket();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}