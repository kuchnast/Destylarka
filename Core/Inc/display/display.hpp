#ifndef DISPLAY_DISPLAY_HPP
#define DISPLAY_DISPLAY_HPP

#include <io/key_defs.hpp>

#include "stm32f4xx_hal.h"

#define LINES_NUM 4
#define LINE_SIZE 20

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

    static DisplayView display_curr_view;
private:

    void imitationPrinting(char *ptr);
    std::string fitStringToLine(const std::string & str1, char prefix = ' ');
    std::string fitStringsToLine(const std::string & str1, const std::string & str2, char prefix = ' ');
    void printMenu(const std::vector<std::string> &msgs, uint8_t possition, const std::vector<std::string> & add_msg);
    void printMenu(const std::vector<std::string> &msgs, uint8_t possition);

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
