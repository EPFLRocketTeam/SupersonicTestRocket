//
// Created by Michael Ha on 22.03.22.
//

#include "hermesAltimax.h"


void hermesAltimax::write(Packet &packet) {
    packet.write(pinStates[0]);
    packet.write(pinStates[1]);
    packet.write(pinStates[2]);
    packet.write(pinStates[3]);
}

void hermesAltimax::parse(Packet &packet) {
    packet.parse(pinStates[0]);
    packet.parse(pinStates[1]);
    packet.parse(pinStates[2]);
    packet.parse(pinStates[3]);
}

void hermesAltimax::print() const {
    std::cout << "Altimax readings: " << pinStates[0] << pinStates[1] << pinStates[2] << std::endl;
}

bool hermesAltimax::updateTx(std::shared_ptr<Connector> connector) {
    pinStates[0] = rand() & 1;
    pinStates[1] = rand() & 1;
    pinStates[2] = rand() & 1;
    pinStates[3] = 0;   // padding byte

    return true;
}

bool hermesAltimax::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesAltimax::log() const {
    return std::move("Altimax" + SEPARATOR +
                     std::to_string(pinStates[0]) +
                     std::to_string(pinStates[1]) +
                     std::to_string(pinStates[2]) + SEPARATOR);
}