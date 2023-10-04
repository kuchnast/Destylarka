#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"

#define DS_SENSORS_NUM 6
#define RELAY_AC_LOW_NUM 8
#define RELAY_AC_HIGH_NUM 3
#define RELAY_DC_AC_NUM 4

/**
 * Wyswietlacz
 * Likwidujemy zbiornik pod pianką
 * 4 linia temeratur 2 razem chlodnice
 *
 * Po przekroczeniu temp 40 stopni kolumna dół załącz pompa chłodnica
 * Gdy temperatura zbiornika osiągną 99,5 stopni wyłącz grzałki i chłodzenie za pięć minut, załączyć buzzer
 * Wpisania temperatury stałej kolumna góra np. 77,8 stopnia i załączenie:
 * 	LM i precyzyjny otw na 30 sekund, 60 s praca, 30 sekund zamykanie, 900 sekund przerwa
 * 	Po przekroczeniu temperatury o 0,2 C przez 60 sekund w górę, lekko przymknąć zawór VM (2 sekundy), odczekać 60 sekund i znów zacząć badać
 * 	Wyjście z pentli gdy zamykany 15 razy albo wzrost o 2 stopnie, załączenie buzzera
 *
 * Temperatura powrót powyżej 40 stopni włącz buzzer, powyżej 50 stopni wyłącz grzałki i zamknij wszystkie zawory (pełne zamknięcie 30-35 sekund)
 *
 * Dorobić miejsce na DSa i sterowanie wiatrakiem
 *
 */

typedef enum
{
	DS_UNKNOWN = 0,
    DS_ZBIORNIK_POD_PIANKA,
    DS_ZBIORNIK_W_KAPILARZE,
    DS_KOLUMNA_DOL,
    DS_KOLUMNA_GORA,
    DS_CHLODNICA_ZASILANIE,
    DS_CHLODNICA_POWROT,
} DsNamesId;

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

typedef enum
{
	R_AC_HIGH_GRZALKA_1=0,
	R_AC_HIGH_GRZALKA_2,
	R_AC_HIGH_POMPA_WODY,
} RelayACHighId;

typedef struct
{
    DsNamesId name_id;
	uint8_t address[8]; ///< one extra byte for null character at the end
} DsSensor;

typedef enum
{
    R_DC_AC_PUSTY_1 = 0,
    R_DC_AC_PUSTY_2,
    R_DC_AC_PUSTY_3,
    R_DC_AC_PUSTY_4,
} RelayDCACId;



typedef struct
{
    RelayACLowId name_id;
    GPIO_TypeDef * gpio;
    uint16_t pin;
} RelayACLow;

typedef struct
{
   const char* text;
   const RelayACLow* dev;
} RelayACLowDisplay;

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

typedef struct
{
    RelayDCACId name_id;
    GPIO_TypeDef * gpio;
    uint16_t pin;
} RelayDCAC;

static const DsSensor known_ds_sensors[DS_SENSORS_NUM] = {
    {DS_ZBIORNIK_POD_PIANKA, {0x28, 0x12, 0x26, 0x9e, 0x0d, 0x00, 0x00, 0xab}},
    {DS_ZBIORNIK_W_KAPILARZE, {0x28, 0xa6, 0xc6, 0x9d, 0x0d, 0x00, 0x00, 0x22}},
    {DS_KOLUMNA_DOL, {0x28, 0x24, 0x4e, 0x9d, 0x0d, 0x00, 0x00, 0x53}},
    {DS_KOLUMNA_GORA, {0x28, 0x17, 0xbc, 0x9e, 0x0d, 0x00, 0x00, 0x4a}},
    {DS_CHLODNICA_ZASILANIE, {0x28, 0xd8, 0xa7, 0x9e, 0x0d, 0x00, 0x00, 0xfd}},
    {DS_CHLODNICA_POWROT, {0x28, 0xe0, 0xa9, 0x9d, 0x0d, 0x00, 0x00, 0x9b}}
};

static const RelayACLow known_ac_low_relays[RELAY_AC_LOW_NUM] = {
    {R_AC_LOW_ZAWOR_VM_ODBIORU_OTW, O_AC_1_GPIO_Port, O_AC_1_Pin},
    {R_AC_LOW_ZAWOR_VM_ODBIORU_ZAM, O_AC_2_GPIO_Port, O_AC_2_Pin},
    {R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_OTW, O_AC_3_GPIO_Port, O_AC_3_Pin},
    {R_AC_LOW_ZAWOR_LM_PLUS_PRECYZYJNY_ZAM, O_AC_4_GPIO_Port, O_AC_4_Pin},
    {R_AC_LOW_ZAWOR_LM_SAM_OTW, O_AC_5_GPIO_Port, O_AC_5_Pin},
    {R_AC_LOW_ZAWOR_LM_SAM_ZAM, O_AC_6_GPIO_Port, O_AC_6_Pin},
    {R_AC_LOW_LAMPA, O_AC_7_GPIO_Port, O_AC_7_Pin},
    {R_AC_LOW_WOLNY, O_AC_8_GPIO_Port, O_AC_8_Pin}
};

static const RelayACHigh known_ac_high_relays[RELAY_AC_HIGH_NUM] = {
    {R_AC_HIGH_POMPA_WODY, O_H_1_GPIO_Port, O_H_1_Pin},
    {R_AC_HIGH_GRZALKA_2, O_H_2_GPIO_Port, O_H_2_Pin},
    {R_AC_HIGH_GRZALKA_1, O_H_3_GPIO_Port, O_H_3_Pin}
};

static const RelayDCAC known_dc_ac_relays[RELAY_DC_AC_NUM] = {
    {R_DC_AC_PUSTY_1, O_DC_1_GPIO_Port, O_DC_1_Pin},
    {R_DC_AC_PUSTY_2, O_DC_2_GPIO_Port, O_DC_2_Pin},
    {R_DC_AC_PUSTY_3, O_DC_3_GPIO_Port, O_DC_3_Pin},
    {R_DC_AC_PUSTY_4, O_DC_4_GPIO_Port, O_DC_4_Pin}
};

#endif
