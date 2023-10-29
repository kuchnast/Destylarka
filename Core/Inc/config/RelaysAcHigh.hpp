#ifndef CONFIG_RELAYS_AC_HIGH_HPP_
#define CONFIG_RELAYS_AC_HIGH_HPP_

#include "io/Relays.hpp"
#include <main.hpp>

namespace io {

enum class RelayACHighId
{
	GRZALKA_1,
	GRZALKA_2,
	POMPA_WODY,
};

static RelaysCollection<RelayACHighId> ac_high_relays({
    {RelayACHighId::POMPA_WODY, io::gpio_pin(O_H_1_GPIO_Port, O_H_1_Pin)},
    {RelayACHighId::GRZALKA_2, io::gpio_pin(O_H_2_GPIO_Port, O_H_2_Pin)},
    {RelayACHighId::GRZALKA_1, io::gpio_pin(O_H_3_GPIO_Port, O_H_3_Pin)}
});

std::string toString(RelayACHighId id)
{
    switch (id)
    {
    case RelayACHighId::GRZALKA_1:
        return "Grzalka 1";
    case RelayACHighId::GRZALKA_2:
        return "Grzalka 2";
    case RelayACHighId::POMPA_WODY:
        return "Pompa wody";
    }
}

} // namespace config

#endif