//
// Created by Michael Ha on 22.03.22.
//

#ifndef ERT2020GS_HERMESHONEYWELLRSC_TEMP_H
#define ERT2020GS_HERMESHONEYWELLRSC_TEMP_H


#include "Data.h"

class hermesHoneywellRsc_Temp : public Data {
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;

    void print() const override;
    std::string log() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float temperature; /// [°C], 4 bytes
};


#endif //ERT2020GS_HERMESHONEYWELLRSC_TEMP_H
