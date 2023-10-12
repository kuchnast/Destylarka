#ifndef CONFIG_RELAYS_DC_AC_HPP_
#define CONFIG_RELAYS_DC_AC_HPP_

#include <io/OutputPin.hpp>

#include <array>
#include <string>

namespace config
{

enum class RelayDCACId
{
    PUSTY_1 = 0,
    PUSTY_2,
    PUSTY_3,
    PUSTY_4,
};

using RelayDCAC = Relay<RelayDCACId>;
using RelayDCACCollection = RelaysCollection<RelayDCAC>;

static const RelayDCACCollection dc_ac_relays({
    {RelayDCACId::PUSTY_1, io::OutputPin(O_DC_1_GPIO_Port, O_DC_1_Pin)},
    {RelayDCACId::PUSTY_2, io::OutputPin(O_DC_2_GPIO_Port, O_DC_2_Pin)},
    {RelayDCACId::PUSTY_3, io::OutputPin(O_DC_3_GPIO_Port, O_DC_3_Pin)},
    {RelayDCACId::PUSTY_4, io::OutputPin(O_DC_4_GPIO_Port, O_DC_4_Pin)},
});

std::string toString(RelayDCACId id)
{
    switch (id)
    {
    case RelayDCACId::PUSTY_1:
        return "Pusty 1";
    case RelayDCACId::PUSTY_2:
        return "Pusty 2";
    case RelayDCACId::PUSTY_3:
        return "Pusty 3";
    case RelayDCACId::PUSTY_4:
        return "Pusty 4";
    }
}

} // namespace config

#endif
