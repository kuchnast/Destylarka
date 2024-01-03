#ifndef CONFIG_RELAYS_AC_LOW_HPP_
#define CONFIG_RELAYS_AC_LOW_HPP_

#include "io/Relays.hpp"
#include <main.hpp>

namespace config
{

enum class RelayACLowId
{
    ZAWOR_VM_ODBIORU_OTWARCIE = 0,
    ZAWOR_VM_ODBIORU_ZAMKNIECIE,
    ZAWOR_LM_PLUS_PRECYZYJNY_OTWARCIE,
    ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE,
    ZAWOR_LM_SAM_OTWARCIE,
    ZAWOR_LM_SAM_ZAMKNIECIE,
    LAMPA,
    WOLNY,
};

static io::RelaysCollection<RelayACLowId> ac_low_relays({
    {RelayACLowId::WOLNY, io::GpioPin(O_AC_1_GPIO_Port, O_AC_1_Pin)},
    {RelayACLowId::LAMPA, io::GpioPin(O_AC_2_GPIO_Port, O_AC_2_Pin)},
    {RelayACLowId::ZAWOR_LM_SAM_ZAMKNIECIE, io::GpioPin(O_AC_3_GPIO_Port, O_AC_3_Pin)},
    {RelayACLowId::ZAWOR_LM_SAM_OTWARCIE, io::GpioPin(O_AC_4_GPIO_Port, O_AC_4_Pin)},
    {RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE, io::GpioPin(O_AC_5_GPIO_Port, O_AC_5_Pin)},
    {RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_OTWARCIE, io::GpioPin(O_AC_6_GPIO_Port, O_AC_6_Pin)},
    {RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE, io::GpioPin(O_AC_7_GPIO_Port, O_AC_7_Pin)},
    {RelayACLowId::ZAWOR_VM_ODBIORU_OTWARCIE, io::GpioPin(O_AC_8_GPIO_Port, O_AC_8_Pin)}
});


std::string toString(RelayACLowId id)
{
    switch (id)
    {
    case RelayACLowId::ZAWOR_VM_ODBIORU_OTWARCIE:
        return "Zawor VM odbior otwarcie";
    case RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE:
        return "Zawor VM odbior zamkniecie";
    case RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_OTWARCIE:
        return "Zawor LM + precyzyjny otwarcie";
    case RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE:
        return "Zawor LM + precyzyjny zamkniecie";
    case RelayACLowId::ZAWOR_LM_SAM_OTWARCIE:
        return "Zawor LM otwarcie";
    case RelayACLowId::ZAWOR_LM_SAM_ZAMKNIECIE:
        return "Zawor LM zamkniecie";
    case RelayACLowId::LAMPA:
        return "Lampa";
    case RelayACLowId::WOLNY:
        return "Wolny";
    }

    return {};
}

} // namespace config

#endif
