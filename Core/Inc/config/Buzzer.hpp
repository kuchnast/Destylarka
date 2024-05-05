#ifndef CONFIG_BUZZER_HPP_
#define CONFIG_BUZZER_HPP_

#include <io/Buzzer.hpp>

namespace config {

static io::Buzzer buzzer(io::GpioPin(BUZZER_GPIO_Port, BUZZER_Pin));
static io::Buzzer buzzer(io::GpioPin(BUZZER_GPIO_Port, BUZZER_Pin));
}
#endif
