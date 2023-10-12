#include <io/OutputPin.hpp>

#include <stm32f4xx_hal_gpio.h>

namespace io
{

void OutputPin::Write(bool state)
{
    HAL_GPIO_WritePin(gpio_, pin_, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool OutputPin::Read()
{
    return HAL_GPIO_ReadPin(gpio_, pin_) == GPIO_PIN_SET ? true : false;
}

void OutputPin::Toggle()
{
    HAL_GPIO_TogglePin(gpio_, pin_);
}

}  // namespace io
