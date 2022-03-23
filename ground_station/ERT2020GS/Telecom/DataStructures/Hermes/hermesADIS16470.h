//
// Created by Michael Ha on 23.03.22.
//

#ifndef ERT2020GS_HERMESADIS16470_H
#define ERT2020GS_HERMESADIS16470_H

#include "Data.h"

class hermesADIS16470 : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float gyros[3]; /// Angular velocities [deg/s] around X,Y,Z axis; 3 * 4 = 12 bytes
    float acc[3];   /// Linear acceleration [g] along X,Y,Z axis; 3 * 4 = 12 bytes
    float temp;     /// Temperature [degC]; 4 bytes
};


#endif //ERT2020GS_HERMESADIS16470_H
