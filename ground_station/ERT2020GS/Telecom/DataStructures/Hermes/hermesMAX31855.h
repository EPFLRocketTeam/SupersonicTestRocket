//
// Created by Michael Ha on 22.03.22.
//

#ifndef ERT2020GS_HERMESMAX31855_H
#define ERT2020GS_HERMESMAX31855_H

#include <Data.h>

class hermesMAX31855 : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float probeTemperature;  ///< [degC]; 4 bytes
    float sensorTemperature; ///< [degC]; 4 bytes
};


#endif //ERT2020GS_HERMESMAX31855_H
