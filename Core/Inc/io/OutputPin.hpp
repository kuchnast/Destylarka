#ifndef IO_OUTPUTPIN_HPP_
#define IO_OUTPUTPIN_HPP_

#include "stm32f401xe.h"

namespace io
{

class OutputPin
{
public:

OutputPin(GPIO_TypeDef * gpio, uint16_t pin) : gpio_(gpio), pin_(pin) {}

void Write(bool state);

bool Read();

void Toggle();

private:
    GPIO_TypeDef * gpio_;
    uint16_t pin_;
};

}  // namespace io

#endif
