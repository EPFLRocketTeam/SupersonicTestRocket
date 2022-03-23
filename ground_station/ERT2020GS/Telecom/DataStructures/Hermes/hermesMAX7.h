/*!
 * \file hermesMAX7.h
 *
 * \brief HERMES module interface
 *
 * \author      HA Michael - EPFL EL BA6
 * \date        22.03.2022
 */

#ifndef ERT2020GS_HERMESMAX7_H
#define ERT2020GS_HERMESMAX7_H

#include <ctime>
#include <Data.h>

class hermesMAX7 : public Data {
public:
    hermesMAX7();
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    uint32_t latitude;  ///< [deg * 10^-7], 8 bytes
    uint32_t longitude; ///< [deg * 10^-7], 8 bytes
    uint32_t altitude;  ///< [mm], 8 bytes
};


#endif //ERT2020GS_HERMESMAX7_H
