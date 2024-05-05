#ifndef IO_ALARMWITHNOTIFICATION_HPP
#define IO_ALARMWITHNOTIFICATION_HPP

#include "io/Buzzer.hpp"

namespace io {

class AlarmWithNotification
{

public:
    AlarmWithNotification(GpioPin buzeer_pin, GpioPin led_pin);

    void enable(const std::string& source, const std::string& reason);

    void disable();

    bool is_enabled();

    void mute();

    void unmute();

    bool is_muted();

    const std::string & getReason();

    const std::string & getSource();

private:
    Buzzer buzzer_;
    GpioPin led_pin_;
    std::string reason_;
    std::string source_;
    bool is_enabled_;

};
}  // namespace io

#endif  // IO_ALARMWITHNOTIFICATION_HPP
