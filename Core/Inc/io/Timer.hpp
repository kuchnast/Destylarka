#ifndef IO_TIMER_HPP
#define IO_TIMER_HPP

#include <tim.h>

namespace io {

class Timer {
public:
    Timer(TIM_HandleTypeDef &tim);

private:
    TIM_HandleTypeDef &tim_;
};

}

#endif  // IO_TIMER_HPP
