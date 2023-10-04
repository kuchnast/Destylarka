#ifndef AC_LOW_RELAYS_H_
#define AC_LOW_RELAYS_H_

#include "config/config.h"

#define RELAY_AC_LOW_NUM 8

typedef enum
{
    R_AC_LOW_ZAWOR_VM_ODBIORU_OTW = 0,
    R_AC_LOW_ZAWOR_VM_ODBIORU_ZAM,
    R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_OTW,
    R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_ZAM,
    R_AC_LOW_ZAWOR_LM_SAM_OTW,
    R_AC_LOW_ZAWOR_LM_SAM_ZAM,
    R_AC_LOW_LAMPA,
    R_AC_LOW_WOLNY,
} RelayACLowId;

typedef struct
{
    RelayACLowId name_id;
    OutputPin output_pin;
} RelayACLow;

typedef struct
{
   const char* text;
   const RelayACLow* dev;
} RelayACLowDisplay;

static const RelayACLow known_ac_low_relays[RELAY_AC_LOW_NUM] = {
    {R_AC_LOW_ZAWOR_VM_ODBIORU_OTW, {O_AC_1_GPIO_Port, O_AC_1_Pin}},
    {R_AC_LOW_ZAWOR_VM_ODBIORU_ZAM, {O_AC_2_GPIO_Port, O_AC_2_Pin}},
    {R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_OTW, {O_AC_3_GPIO_Port, O_AC_3_Pin}},
    {R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_ZAM, {O_AC_4_GPIO_Port, O_AC_4_Pin}},
    {R_AC_LOW_ZAWOR_LM_SAM_OTW, {O_AC_5_GPIO_Port, O_AC_5_Pin}},
    {R_AC_LOW_ZAWOR_LM_SAM_ZAM, {O_AC_6_GPIO_Port, O_AC_6_Pin}},
    {R_AC_LOW_LAMPA, {O_AC_7_GPIO_Port, O_AC_7_Pin}},
    {R_AC_LOW_WOLNY, {O_AC_8_GPIO_Port, O_AC_8_Pin}}
};

#endif
