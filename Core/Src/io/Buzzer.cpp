
#include "io/Buzzer.hpp"

namespace io {

Buzzer::Buzzer(GpioPin gpio_pin) : gpio_pin_(gpio_pin), muted_(false)
{
}

void Buzzer::enable()
{
    if(!muted_)
        gpio_pin_.reset();
}

void Buzzer::disable()
{
    gpio_pin_.set();
}

void Buzzer::mute()
{
    disable();
    muted_ = true;
}

void Buzzer::unmute()
{
    muted_ = false;
}

bool Buzzer::is_muted()
{
    return muted_;
}

}  // namespace io
