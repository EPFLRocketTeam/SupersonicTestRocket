/*!
 * \file hermesMAX7.cpp
 *
 * \brief HERMES module implementation
 *
 * \author      HA Michael - EPFL EL BA6
 * \date        22.03.2022
 */

#include "hermesMAX7.h"

#include <iomanip>
#include <Loggable.h>
#include <cmath>

hermesMAX7::hermesMAX7() {
    srand(std::time(nullptr)); // for simulation random
}

void hermesMAX7::write(Packet& packet) {
    packet.write(latitude);
    packet.write(longitude);
    packet.write(altitude);
}


void hermesMAX7::parse(Packet& packet) {
    packet.parse(latitude);
    packet.parse(longitude);
    packet.parse(altitude);
}

void hermesMAX7::print() const {
    std::cout << std::setprecision(7) << std::fixed;
    std::cout << "----- GPS DATA --------------" << std::endl;
    std::cout << "latitude : " << latitude*pow(10,-7) << "°" << std::endl
              << "longitude : " << longitude*pow(10,-7) << "°" << std::endl
              << "altitude : " << altitude/1000 << " m" << std::endl;
}

bool hermesMAX7::updateTx(std::shared_ptr<Connector> connector) {
    latitude =  2;
    longitude =  4;
    altitude =  999000;
    return true;
}

bool hermesMAX7::updateRx(std::shared_ptr<Connector> connector) {
    connector->setData(ui_interface::GPS_LATITUDE, latitude);
    connector->setData(ui_interface::GPS_LONGITUDE, longitude);
    connector->setData(ui_interface::GPS_ALTITUDE, altitude);
    if (altitude > connector->getData<float>(ui_interface::ALTITUDE_MAX))
        connector->setData(ui_interface::ALTITUDE_MAX, altitude);
    return true;
}

std::string hermesMAX7::log() const {
    return std::move( "GPS" + SEPARATOR +
                      std::to_string(latitude) + SEPARATOR +
                      std::to_string(longitude) + SEPARATOR +
                      std::to_string(altitude) + SEPARATOR);
}
