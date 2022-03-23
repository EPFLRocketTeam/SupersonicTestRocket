//
// Created by Michael Ha on 22.03.22.
//

#ifndef ERT2020GS_HERMESHONEYWELLRSC_PRESSURE_H
#define ERT2020GS_HERMESHONEYWELLRSC_PRESSURE_H

#include "Data.h"

class hermesHoneywellRsc_Pressure : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float pressure; /// [bar], 4 bytes
};


#endif //ERT2020GS_HERMESHONEYWELLRSC_PRESSURE_H
