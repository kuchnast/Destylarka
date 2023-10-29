#include <io/GpioPin.hpp>

#include <stm32f4xx_hal_gpio.h>

namespace io
{

void GpioPin::write(PinState state)
{
    HAL_GPIO_WritePin(gpio_, pin_, state == PinState::SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

PinState GpioPin::read()
{
    return HAL_GPIO_ReadPin(gpio_, pin_) == GPIO_PIN_SET ? PinState::SET : PinState::RESET;
}

void GpioPin::set()
{
    write(PinState::SET);
}

void GpioPin::reset()
{
    write(PinState::RESET);
}

void GpioPin::toggle()
{
    HAL_GPIO_TogglePin(gpio_, pin_);
}

}  // namespace io
