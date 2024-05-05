#ifndef CONFIG_BUZZER_HPP_
#define CONFIG_BUZZER_HPP_

#include <io/AlarmWithNotification.hpp>

namespace config {

static io::AlarmWithNotification alarm_with_notification(io::GpioPin(BUZZER_GPIO_Port, BUZZER_Pin),
    io::GpioPin(NOTIFICATION_LED_GPIO_Port, NOTIFICATION_LED_Pin));
}
#endif
