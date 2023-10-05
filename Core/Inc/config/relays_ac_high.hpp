#ifndef CONFIG_RELAYS_AC_HIGH_HPP_
#define CONFIG_RELAYS_AC_HIGH_HPP_

#include <config/relays.hpp>

#include <io/OutputPin.hpp>

#include <array>
#include <string>

namespace config
{

#define RELAY_AC_HIGH_NUM 3

enum class RelayACHighId
{
	GRZALKA_1 = 0,
	GRZALKA_2,
	POMPA_WODY,
};

using RelayACHigh = Relay<RelayACHighId>;
using RelayACHighDisplay = RelayDisplay<RelayACHighId>;
using RelayACHighCollection = RelaysCollection<RelayACHigh, RELAY_AC_HIGH_NUM>;

const std::array<const RelayACHighId, RELAY_AC_HIGH_NUM> RelayACHighCollection::relays =
{
    RelayACHigh(RelayACHighId::POMPA_WODY, io::OutputPin(O_H_1_GPIO_Port, O_H_1_Pin)),
    RelayACHigh(RelayACHighId::POMPA_WODY, io::OutputPin(O_H_1_GPIO_Port, O_H_1_Pin)),
    RelayACHigh(RelayACHighId::POMPA_WODY, io::OutputPin(O_H_1_GPIO_Port, O_H_1_Pin))
};

const RelayACHighCollection ac_high_relays = {
    {RelayACHighId::POMPA_WODY, io::OutputPin(O_H_1_GPIO_Port, O_H_1_Pin)},
    {RelayACHighId::GRZALKA_2, io::OutputPin(O_H_2_GPIO_Port, O_H_2_Pin)},
    {RelayACHighId::GRZALKA_1, io::OutputPin(O_H_3_GPIO_Port, O_H_3_Pin)}
};

} // namespace config

#endif
