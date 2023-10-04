#ifndef AC_HIGH_RELAYS_H_
#define AC_HIGH_RELAYS_H_

#include "config/config.h"

#define RELAY_AC_HIGH_NUM 3

typedef enum
{
	R_AC_HIGH_GRZALKA_1=0,
	R_AC_HIGH_GRZALKA_2,
	R_AC_HIGH_POMPA_WODY,
} RelayACHighId;

typedef struct
{
    RelayACHighId name_id;
    GPIO_TypeDef * gpio;
    uint16_t pin;
} RelayACHigh;

typedef struct
{
   const char* text;
   const RelayACHigh* dev;
} RelayACHighDisplay;

static const RelayACHigh known_ac_high_relays[RELAY_AC_HIGH_NUM] = {
    {R_AC_HIGH_POMPA_WODY, O_H_1_GPIO_Port, O_H_1_Pin},
    {R_AC_HIGH_GRZALKA_2, O_H_2_GPIO_Port, O_H_2_Pin},
    {R_AC_HIGH_GRZALKA_1, O_H_3_GPIO_Port, O_H_3_Pin}
};

#endif
