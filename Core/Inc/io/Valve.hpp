#ifndef SENSORS_VALVE_HPP
#define SENSORS_VALVE_HPP

#include "GpioPin.hpp"
#include "Relays.hpp"

namespace io {

template<typename T>
class Valve {
    static_assert(std::is_enum_v<T>, "T must be enum");

public:
    Valve(Relay<T>& open_pin, Relay<T>& close_pin, uint16_t time_ms);

    void open();

    void close();

private:
    Relay<T> &open_pin_;
    Relay<T> &close_pin_;
    uint16_t time_ms_;
};

template<typename T>
Valve<T>::Valve(Relay<T> &open_pin, Relay<T> &close_pin, uint16_t time_ms) {
}

}  // namespace io

#endif  // SENSORS_VALVE_HPP
