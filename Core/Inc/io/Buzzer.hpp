#ifndef IO_BUZZER_HPP
#define IO_BUZZER_HPP

#include "GpioPin.hpp"
#include "Relays.hpp"
#include "io/FunctionTimer.hpp"

namespace io {

class Buzzer {

public:
    Buzzer(GpioPin gpio_pin);

    void enable();

    void disable();

    void mute();

    void unmute();

    bool is_muted();

private:
    GpioPin gpio_pin_;
    bool muted_;

};
}  // namespace io

#endif  // IO_BUZZER_HPP
