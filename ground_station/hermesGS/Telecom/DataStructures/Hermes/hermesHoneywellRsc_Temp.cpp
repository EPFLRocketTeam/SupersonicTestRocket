//
// Created by Michael Ha on 22.03.22.
//

#include "hermesHoneywellRsc_Temp.h"

void hermesHoneywellRsc_Temp::write(Packet &packet) {
    packet.write(temperature);
}

void hermesHoneywellRsc_Temp::parse(Packet &packet) {
    packet.parse(temperature);
}

void hermesHoneywellRsc_Temp::print() const {
    std::cout << "temperature: " << temperature << "°C" << std::endl;
}

bool hermesHoneywellRsc_Temp::updateTx(std::shared_ptr<Connector> connector) {
    temperature =  24 + ((float) rand()/ RAND_MAX);
    return true;
}

bool hermesHoneywellRsc_Temp::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesHoneywellRsc_Temp::log() const {
    return std::move("HoneywellRsc_Temp" + SEPARATOR +
                     std::to_string(temperature) + SEPARATOR);
}