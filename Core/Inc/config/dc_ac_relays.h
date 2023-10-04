#ifndef DC_AC_RELAYS_H_
#define DC_AC_RELAYS_H_

#include "config/config.h"

#define RELAY_DC_AC_NUM 4

typedef enum
{
    R_DC_AC_PUSTY_1 = 0,
    R_DC_AC_PUSTY_2,
    R_DC_AC_PUSTY_3,
    R_DC_AC_PUSTY_4,
} RelayDCACId;

typedef struct
{
    RelayDCACId name_id;
    GPIO_TypeDef * gpio;
    uint16_t pin;
} RelayDCAC;

static const RelayDCAC known_dc_ac_relays[RELAY_DC_AC_NUM] = {
    {R_DC_AC_PUSTY_1, O_DC_1_GPIO_Port, O_DC_1_Pin},
    {R_DC_AC_PUSTY_2, O_DC_2_GPIO_Port, O_DC_2_Pin},
    {R_DC_AC_PUSTY_3, O_DC_3_GPIO_Port, O_DC_3_Pin},
    {R_DC_AC_PUSTY_4, O_DC_4_GPIO_Port, O_DC_4_Pin}
};

#endif
