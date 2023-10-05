#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
#include <main.hpp>
#include "gpio.h"

#define           _KEYPAD_DEBOUNCE_TIME_MS        20
#define           _KEYPAD_USE_FREERTOS            0

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{
  K_COL_1_GPIO_Port,
  K_COL_2_GPIO_Port,
  K_COL_3_GPIO_Port,
  K_COL_4_GPIO_Port
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  K_COL_1_Pin,
  K_COL_2_Pin,
  K_COL_3_Pin,
  K_COL_4_Pin
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
  K_ROW_1_GPIO_Port,
  K_ROW_2_GPIO_Port,
  K_ROW_3_GPIO_Port,
  K_ROW_4_GPIO_Port,
  K_ROW_5_GPIO_Port
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
  K_ROW_1_Pin,
  K_ROW_2_Pin,
  K_ROW_3_Pin,
  K_ROW_4_Pin,
  K_ROW_5_Pin
};

#endif
