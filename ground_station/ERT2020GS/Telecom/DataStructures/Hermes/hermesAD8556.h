//
// Created by Michael Ha on 26.03.22.
//

#ifndef ERT2020GS_HERMESAD8556_H
#define ERT2020GS_HERMESAD8556_H

#include "Data.h"

class hermesAD8556 : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float thrust; /// [N], 4 bytes
};

#endif //ERT2020GS_HERMESAD8556_H
