#ifndef SENSORS_DS_HPP_
#define SENSORS_DS_HPP_

#include <stdint.h>

#include <array>
#include <vector>

enum class DsNameId
{
	UNKNOWN = 0,
    ZBIORNIK_POD_PIANKA,
    ZBIORNIK_W_KAPILARZE,
    KOLUMNA_DOL,
    KOLUMNA_GORA,
    CHLODNICA_ZASILANIE,
    CHLODNICA_POWROT,
};

struct DsSensor
{
    DsNameId name_id;
	std::array<uint8_t, 8> address;
};

using DsSensorsCollection = std::vector<DsSensor>;

static const DsSensorsCollection ds_sensors = {
    {DsNameId::ZBIORNIK_POD_PIANKA, {0x28, 0x12, 0x26, 0x9e, 0x0d, 0x00, 0x00, 0xab}},
    {DsNameId::ZBIORNIK_W_KAPILARZE, {0x28, 0xa6, 0xc6, 0x9d, 0x0d, 0x00, 0x00, 0x22}},
    {DsNameId::KOLUMNA_DOL, {0x28, 0x24, 0x4e, 0x9d, 0x0d, 0x00, 0x00, 0x53}},
    {DsNameId::KOLUMNA_GORA, {0x28, 0x17, 0xbc, 0x9e, 0x0d, 0x00, 0x00, 0x4a}},
    {DsNameId::CHLODNICA_ZASILANIE, {0x28, 0xd8, 0xa7, 0x9e, 0x0d, 0x00, 0x00, 0xfd}},
    {DsNameId::CHLODNICA_POWROT, {0x28, 0xe0, 0xa9, 0x9d, 0x0d, 0x00, 0x00, 0x9b}}
};

#endif
