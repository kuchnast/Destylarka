#ifndef RELAYS_HPP_
#define RELAYS_HPP_

#include <io/OutputPin.hpp>

#include <string>
#include <vector>

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
class RelaysCollection
{
static_assert(std::is_enum_v<T>, "T must be enum");

public:
    RelaysCollection(const std::vector<T> relays) : relays(relays) {}

    size_t Size() const
    {
        return relays.size();
    }

    io::OutputPin &Find(T id)
    {
        for(Relay<T> &relay: relays)
        {
            if(relay.id == id)
                return relay.pin;
        }
    }

    const std::vector<Relay<T>> relays;
};

} // namespace config

#endif
