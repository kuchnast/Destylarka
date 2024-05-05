#include "io/AlarmWithNotification.hpp"

namespace io {

AlarmWithNotification::AlarmWithNotification(GpioPin buzeer_pin, GpioPin led_pin) : buzzer_(buzeer_pin), led_pin_(led_pin), is_enabled_(false)
{
}

void AlarmWithNotification::enable(const std::string& source, const std::string& reason)
{
    buzzer_.enable();
    led_pin_.set();
    source_ = source;
    reason_ = reason;
    is_enabled_ = true;
}

void AlarmWithNotification::disable()
{
    buzzer_.disable();
    led_pin_.reset();
    source_.clear();
    reason_.clear();
    is_enabled_ = false;
}

bool AlarmWithNotification::is_enabled()
{
    return is_enabled_;
}

void AlarmWithNotification::mute()
{
    buzzer_.mute();
}

void AlarmWithNotification::unmute()
{
    buzzer_.unmute();
}

bool AlarmWithNotification::is_muted()
{
    return buzzer_.is_muted();
}

const std::string & AlarmWithNotification::getReason()
{
    return reason_;
}

const std::string & AlarmWithNotification::getSource()
{
    return source_;
}

}  // namespace io
