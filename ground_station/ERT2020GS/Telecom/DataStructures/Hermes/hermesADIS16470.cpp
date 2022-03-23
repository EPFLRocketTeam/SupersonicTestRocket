//
// Created by Michael Ha on 23.03.22.
//

#include "hermesADIS16470.h"

void hermesADIS16470::write(Packet &packet) {
    packet.write(gyros[0]);
    packet.write(gyros[1]);
    packet.write(gyros[2]);
    packet.write(acc[0]);
    packet.write(acc[1]);
    packet.write(acc[2]);
    packet.write(temp);
}

void hermesADIS16470::parse(Packet &packet) {
    packet.parse(gyros[0]);
    packet.parse(gyros[1]);
    packet.parse(gyros[2]);
    packet.parse(acc[0]);
    packet.parse(acc[1]);
    packet.parse(acc[2]);
    packet.parse(temp);
}

void hermesADIS16470::print() const {
    std::cout << "xgyro: " << gyros[0] << " ygyro: " << gyros[1] << " zgyro: " << gyros[2] << " [deg/sec]" << std::endl;
    std::cout << "xaccel: " << acc[0] << " yaccel " << acc[1] << " zaccel " << acc[2] << " [g] " << std::endl;
    std::cout << "temperature: " << temp << " [°C]" << std::endl;
}

bool hermesADIS16470::updateTx(std::shared_ptr<Connector> connector) {

    return true;
}

bool hermesADIS16470::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesADIS16470::log() const {
    return std::move("ADIS16470" + SEPARATOR +
                     std::to_string(gyros[0]) + SEPARATOR +
                     std::to_string(gyros[1]) + SEPARATOR +
                     std::to_string(gyros[2]) + SEPARATOR +
                     std::to_string(acc[0]) + SEPARATOR +
                     std::to_string(acc[1]) + SEPARATOR +
                     std::to_string(acc[2]) + SEPARATOR +
                     std::to_string(temp) + SEPARATOR);
}