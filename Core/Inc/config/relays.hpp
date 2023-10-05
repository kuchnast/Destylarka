#ifndef RELAYS_HPP_
#define RELAYS_HPP_

#include <io/OutputPin.hpp>

#include <array>
#include <string>

namespace config
{

template <typename T>
struct Relay
{
    static_assert(std::is_enum_v<T>, "T must be enum");

    Relay(T id, io::OutputPin pin) : id(id), pin(pin) {}

    T id;
    io::OutputPin pin;
};

template <typename T>
struct RelayDisplay
{
    static_assert(std::is_base_of_v<Relay, T>, "T must derive from Relay");

    const T& config;
    const std::string display_str;
};

template <typename T, std::size_t N>
struct RelaysCollection
{
    static_assert(std::is_base_of_v<Relay, T>, "T must derive from Relay");

    static const std::array<const T, N> relays; 
};

} // namespace config

#endif
