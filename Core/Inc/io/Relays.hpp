#ifndef RELAYS_HPP_
#define RELAYS_HPP_

#include "GpioPin.hpp"

#include <string>
#include <vector>

namespace io {

template <typename T>
struct Relay
{
    static_assert(std::is_enum_v<T>, "T must be enum");

    Relay(T id, io::GpioPin pin) : id(id), pin(pin) {}

    T id;
    io::GpioPin pin;
};

template <typename T>
class RelaysCollection
{
static_assert(std::is_enum_v<T>, "T must be enum");

public:
    explicit RelaysCollection(const std::vector<Relay<T>> &relays) : relays_(relays) {}

    [[nodiscard]] size_t Size() const
    {
        return relays_.size();
    }

    io::GpioPin &Find(T id)
    {
        for(Relay<T> &relay: relays_)
        {
            if(relay.id == id)
                return relay.pin;
        }
    }

private:
    std::vector<Relay<T>> relays_;
};

} // namespace io

#endif
