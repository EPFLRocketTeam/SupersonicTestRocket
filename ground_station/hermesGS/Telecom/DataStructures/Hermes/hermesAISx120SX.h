//
// Created by Michael Ha on 22.03.22.
//

#ifndef ERT2020GS_HERMESAISX120SX_H
#define ERT2020GS_HERMESAISX120SX_H

#include "Data.h"

class hermesAISx120SX : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float accel[2]; /// Linear acceleration [g] along X,Y axis; 2 * 4 = 8 bytes
};


#endif //ERT2020GS_HERMESAISX120SX_H
