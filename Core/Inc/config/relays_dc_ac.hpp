#ifndef CONFIG_RELAYS_DC_AC_HPP_
#define CONFIG_RELAYS_DC_AC_HPP_

#include <io/OutputPin.hpp>

#include <array>
#include <string>

namespace config
{

#define RELAY_DC_AC_NUM 4

enum class RelayDCACId
{
    R_DC_AC_PUSTY_1 = 0,
    R_DC_AC_PUSTY_2,
    R_DC_AC_PUSTY_3,
    R_DC_AC_PUSTY_4,
};

struct RelayDCAC
{
    RelayDCACId id;
    io::OutputPin pin;
};

struct RelayDCACDisplay
{
   const RelayDCAC* config;
   const std::string display_str;

};

const std::array<RelayDCAC, RELAY_DC_AC_NUM> ac_high_relays = {
    {RelayACHighId::POMPA_WODY, {O_H_1_GPIO_Port, O_H_1_Pin}},
    {RelayACHighId::GRZALKA_2, {O_H_2_GPIO_Port, O_H_2_Pin}},
    {RelayACHighId::GRZALKA_1, {O_H_3_GPIO_Port, O_H_3_Pin}}
};

} // namespace config

#endif


typedef enum
{
    R_DC_AC_PUSTY_1 = 0,
    R_DC_AC_PUSTY_2,
    R_DC_AC_PUSTY_3,
    R_DC_AC_PUSTY_4,
} RelayDCACId;

typedef struct
{
    RelayDCACId name_id;
    GPIO_TypeDef * gpio;
    uint16_t pin;
} RelayDCAC;

static const RelayDCAC known_dc_ac_relays[RELAY_DC_AC_NUM] = {
    {R_DC_AC_PUSTY_1, O_DC_1_GPIO_Port, O_DC_1_Pin},
    {R_DC_AC_PUSTY_2, O_DC_2_GPIO_Port, O_DC_2_Pin},
    {R_DC_AC_PUSTY_3, O_DC_3_GPIO_Port, O_DC_3_Pin},
    {R_DC_AC_PUSTY_4, O_DC_4_GPIO_Port, O_DC_4_Pin}
};

#endif
