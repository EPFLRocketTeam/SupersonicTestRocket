//
// Created by Michael Ha on 22.03.22.
//

#include "hermesMAX31855.h"

void hermesMAX31855::write(Packet &packet) {
    packet.write(probeTemperature);
    packet.write(sensorTemperature);
}

void hermesMAX31855::parse(Packet &packet) {
    packet.parse(probeTemperature);
    packet.parse(sensorTemperature);
}

void hermesMAX31855::print() const {
    std::cout << "probe Temperature: " << probeTemperature << "°C" << std::endl
              << "sensor Temperature: " << sensorTemperature << "°C" << std::endl;
}

bool hermesMAX31855::updateTx(std::shared_ptr<Connector> connector) {
    probeTemperature =  12 + ((float) rand()/ RAND_MAX) * 20;
    sensorTemperature =  10 + ((float) rand()/ RAND_MAX) * 20;
    return true;
}

bool hermesMAX31855::updateRx(std::shared_ptr<Connector> connector) {
    //connector->setData(ui_interface::T_TEMPERATURE, temperature);
    return true;
}

std::string hermesMAX31855::log() const {
    return std::move("MAX31855" + SEPARATOR +
                     std::to_string(probeTemperature) + SEPARATOR +
                     std::to_string(sensorTemperature) + SEPARATOR);
}