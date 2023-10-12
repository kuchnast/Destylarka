#ifndef DISPLAY_DISPLAY_HPP
#define DISPLAY_DISPLAY_HPP

#include <io/key_defs.hpp>

#include "stm32f4xx_hal.h"

namespace display
{

enum class DisplayView
{
    EMPTY,
    MAIN_MENU,
    TEMP_SENSORS,
    AC_LOW_RELAYS,
    AC_HIGH_RELAYS,
    DC_AC_RELAYS,
    SET_ALARM
};

extern DisplayView display_curr_view;

struct DisplayViewPos
{
    uint8_t x;
    uint8_t y;

    DisplayViewPos(uint8_t x, uint8_t y) : x(x), y(y) {}
};

class Display
{
public:


void Init();

void Clear();

void ViewAction(Key key);

private:

void imitationPrinting(char *ptr);

void WelcomeScreen();

void MainMenuAction(Key key);

void TempSensorsAction(Key key);

void AcLowRelaysAction(Key key);

void AcHighRelaysAction(Key key);

void DcAcRelaysAction(Key key);

void SetAlarmAction(Key key);

};

}

#endif
