#ifndef DISPLAY_DISPLAY_HPP
#define DISPLAY_DISPLAY_HPP

#include "config/Keypad.hpp"

#include "stm32f4xx_hal.h"

#include <string>
#include <vector>

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
    void init();
    static void clearScreen();
    void viewAction(const config::Key& key);
    const DisplayView& getCurrentView();
    void setCurrentView(const DisplayView& display_view);

private:
    static void imitationPrinting(const std::string& str);
    static std::string fitStringToLine(const std::string & str1, char prefix = ' ');
    static std::string fitStringsToLine(const std::string & str1, const std::string & str2, char prefix = ' ');
    static void printMenu(const std::vector<std::string> &msgs, uint8_t possition, const std::vector<std::string> & add_msg);
    static void printMenu(const std::vector<std::string> &msgs, uint8_t possition);

    void welcomeScreen();
    void mainMenuAction(const config::Key& key);
    void tempSensorsAction(const config::Key& key);
    void acLowRelaysAction(const config::Key& key);
    void acHighRelaysAction(const config::Key& key);
    void dcAcRelaysAction(const config::Key& key);
    void setAlarmAction(const config::Key& key);

    DisplayView current_view_;
};
}

#endif
