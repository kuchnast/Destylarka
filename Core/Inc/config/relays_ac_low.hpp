#ifndef CONFIG_RELAYS_AC_LOW_HPP_
#define CONFIG_RELAYS_AC_LOW_HPP_

#include <io/OutputPin.hpp>

#include <array>
#include <string>

namespace config
{

#define RELAY_AC_LOW_NUM 8

enum class RelayACLowId
{
    ZAWOR_VM_ODBIORU_OTW = 0,
    ZAWOR_VM_ODBIORU_ZAM,
    ZAWOR_LM_PLUS_PRECYZYJNY_OTW,
    ZAWOR_LM_PLUS_PRECYZYJNY_ZAM,
    ZAWOR_LM_SAM_OTW,
    ZAWOR_LM_SAM_ZAM,
    LAMPA,
    WOLNY,
};

struct RelayACLow
{
    RelayACLowId id;
    io::OutputPin pin;
};

struct RelayACLowDisplay
{
   const RelayACLow* config;
   const std::string display_str;
};

const std::array<RelayACLow, RELAY_AC_LOW_NUM> ac_low_relays = {
    {RelayACLowId::ZAWOR_VM_ODBIORU_OTW, {O_AC_1_GPIO_Port, O_AC_1_Pin}},
    {RelayACLowId::ZAWOR_VM_ODBIORU_ZAM, {O_AC_2_GPIO_Port, O_AC_2_Pin}},
    {RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_OTW, {O_AC_3_GPIO_Port, O_AC_3_Pin}},
    {RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAM, {O_AC_4_GPIO_Port, O_AC_4_Pin}},
    {RelayACLowId::ZAWOR_LM_SAM_OTW, {O_AC_5_GPIO_Port, O_AC_5_Pin}},
    {RelayACLowId::ZAWOR_LM_SAM_ZAM, {O_AC_6_GPIO_Port, O_AC_6_Pin}},
    {RelayACLowId::LAMPA, {O_AC_7_GPIO_Port, O_AC_7_Pin}},
    {RelayACLowId::WOLNY, {O_AC_8_GPIO_Port, O_AC_8_Pin}}
};

} // namespace config

#endif
