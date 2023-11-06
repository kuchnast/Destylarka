#ifndef SENSORS_VALVE_HPP
#define SENSORS_VALVE_HPP

#include "GpioPin.hpp"
#include "Relays.hpp"
#include "io/FunctionTimer.hpp"

namespace io {

template<typename T>
class Valve {
    static_assert(std::is_enum_v<T>, "T must be enum");

public:
    Valve(Relay<T>& open_pin, Relay<T>& close_pin, uint32_t time_ms);

    void open();

    void close();

private:
    Relay<T> &open_pin_;
    Relay<T> &close_pin_;
    uint32_t time_ms_;
    uint32_t timer_id_;

};

template<typename T>
Valve<T>::Valve(Relay<T> &open_pin, Relay<T> &close_pin, uint32_t time_ms) :
    open_pin_(open_pin), close_pin_(close_pin), time_ms_(time_ms), timer_id_()
{
}

template<typename T>
void Valve<T>::open()
{
    if(open_pin_.pin.read() == PinState::RESET) // if not opening
    {
        if(close_pin_.pin.read() == PinState::SET)  // if closing - stop and remove timer
        {
            close_pin_.pin.reset();
            FunctionTimer::removeFunction(timer_id_);
        }

        open_pin_.pin.set();
        timer_id_ = FunctionTimer::addFunction([&](){open_pin_.pin.reset();}, time_ms_);
    }
}

template<typename T>
void Valve<T>::close()
{
    if(close_pin_.pin.read() == PinState::RESET) // if not closing
    {
        if(open_pin_.pin.read() == PinState::SET)  // if opening - stop and remove timer
        {
            open_pin_.pin.reset();
            FunctionTimer::removeFunction(timer_id_);
        }

        close_pin_.pin.set();
        timer_id_ = FunctionTimer::addFunction([&](){close_pin_.pin.reset();}, time_ms_);
    }
}

}  // namespace io

#endif  // SENSORS_VALVE_HPP
