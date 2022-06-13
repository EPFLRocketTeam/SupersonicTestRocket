//
// Created by Michael Ha on 22.03.22.
//

#include "hermesHoneywellRsc_Pressure.h"

void hermesHoneywellRsc_Pressure::write(Packet &packet) {
    packet.write(pressure);
}

void hermesHoneywellRsc_Pressure::parse(Packet &packet) {
    packet.parse(pressure);
}

void hermesHoneywellRsc_Pressure::print() const {
    std::cout << "pressure: " << pressure << " bar" << std::endl;
}

bool hermesHoneywellRsc_Pressure::updateTx(std::shared_ptr<Connector> connector) {
    pressure =  1 + ((float) rand()/ RAND_MAX);
    return true;
}

bool hermesHoneywellRsc_Pressure::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesHoneywellRsc_Pressure::log() const {
    return std::move("HoneywellRsc_Pressure" + SEPARATOR +
                     std::to_string(pressure) + SEPARATOR);
}