#ifndef IO_OUTPUTPIN_HPP_
#define IO_OUTPUTPIN_HPP_

#include "main.hpp"

namespace io
{
    enum class PinState
    {
        RESET = 0,
        SET = 1
    };

    class GpioPin {
    public:
        GpioPin(GPIO_TypeDef * gpio, uint16_t pin) : gpio_(gpio), pin_(pin) {}

        void init(GPIO_InitTypeDef &gpio_init);

        void init(uint32_t mode, uint32_t pull, uint32_t speed);

        void write(PinState state);

        void set();

        void reset();

        PinState read();

        void toggle();

    private:
        GPIO_TypeDef * gpio_;
        uint16_t pin_;
    };

}  // namespace io

#endif
