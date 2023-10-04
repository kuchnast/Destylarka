#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"

typedef struct
{
    GPIO_TypeDef * gpio;
    uint16_t pin;
} OutputPin;

#endif
