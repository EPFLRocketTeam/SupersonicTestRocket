//
// Created by Michael Ha on 22.03.22.
//

#include "hermesAISx120SX.h"

void hermesAISx120SX::write(Packet &packet) {
    packet.write(accel[0]);
    packet.write(accel[1]);
}

void hermesAISx120SX::parse(Packet &packet) {
    packet.parse(accel[0]);
    packet.parse(accel[1]);
}

void hermesAISx120SX::print() const {
    std::cout << "xaccel: " << accel[0] << " g" << std::endl;
    std::cout << "yaccel: " << accel[1] << " g" << std::endl;
}

bool hermesAISx120SX::updateTx(std::shared_ptr<Connector> connector) {
    accel[0] = ((float)rand()/RAND_MAX);
    accel[1] = ((float)rand()/RAND_MAX);
    return true;
}

bool hermesAISx120SX::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesAISx120SX::log() const {
    return std::move("AISx120SX" + SEPARATOR +
                     std::to_string(accel[0]) + SEPARATOR +
                     std::to_string(accel[1]) + SEPARATOR);
}