//
// Created by Michael Ha on 26.03.22.
//

#include "hermesAD8556.h"

void hermesAD8556::write(Packet &packet) {
    packet.write(thrust);
}

void hermesAD8556::parse(Packet &packet) {
    packet.parse(thrust);
}

void hermesAD8556::print() const {
    std::cout << "thrust: " << thrust << " N" << std::endl;
}

bool hermesAD8556::updateTx(std::shared_ptr<Connector> connector) {
    thrust =  12 + ((float) rand()/ RAND_MAX) * 4;
    return true;
}

bool hermesAD8556::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesAD8556::log() const {
    return std::move("AD8556" + SEPARATOR +
                     std::to_string(thrust) + SEPARATOR);
}