#ifndef CONFIG_SENSORS_DS18b20_HPP_
#define CONFIG_SENSORS_DS18b20_HPP_

#include <array>
#include <cstdint>
#include <vector>

namespace config {

    enum class Ds18b20NameId {
        ZBIORNIK_POD_PIANKA,
        ZBIORNIK_W_KAPILARZE,
        KOLUMNA_DOL,
        KOLUMNA_GORA,
        CHLODNICA_ZASILANIE,
        CHLODNICA_POWROT,
    };

    struct Ds18b20Sensor {
        Ds18b20NameId name_id;
        std::array<uint8_t, 8> address;
    };

    static const std::vector<Ds18b20Sensor> ds_sensors = {
            {Ds18b20NameId::ZBIORNIK_POD_PIANKA,  {0x28, 0x12, 0x26, 0x9e, 0x0d, 0x00, 0x00, 0xab}},
            {Ds18b20NameId::ZBIORNIK_W_KAPILARZE, {0x28, 0xa6, 0xc6, 0x9d, 0x0d, 0x00, 0x00, 0x22}},
            {Ds18b20NameId::KOLUMNA_DOL,          {0x28, 0x24, 0x4e, 0x9d, 0x0d, 0x00, 0x00, 0x53}},
            {Ds18b20NameId::KOLUMNA_GORA,         {0x28, 0x17, 0xbc, 0x9e, 0x0d, 0x00, 0x00, 0x4a}},
            {Ds18b20NameId::CHLODNICA_ZASILANIE,  {0x28, 0xd8, 0xa7, 0x9e, 0x0d, 0x00, 0x00, 0xfd}},
            {Ds18b20NameId::CHLODNICA_POWROT,     {0x28, 0xe0, 0xa9, 0x9d, 0x0d, 0x00, 0x00, 0x9b}}
    };
}
#endif
