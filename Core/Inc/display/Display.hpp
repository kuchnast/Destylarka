#ifndef DISPLAY_DISPLAY_HPP
#define DISPLAY_DISPLAY_HPP

#include <io/Keypad.hpp>
#include <io/Logger.hpp>

#include <sensors/Ds18b20.hpp>

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

std::string toString(const DisplayView& displayView);

struct DisplayViewPos
{
    uint8_t x;
    uint8_t y;

    DisplayViewPos(uint8_t x, uint8_t y) : x(x), y(y) {}
};

class Display
{

public:
    explicit Display(sensors::Ds18b20Collection& ds_collection);

    void init(I2C_HandleTypeDef *hi2c, uint8_t address = 0x27, uint8_t lines = 4, uint8_t rows = 20);
    static void clearScreen();
    void viewAction(const config::Key& key);
    const DisplayView& getCurrentView();
    void setCurrentView(const DisplayView& display_view);

private:
    static void imitationPrinting(const std::string& str);
    static std::string fitStringToLine(const std::string & str1, char prefix = ' ');
    static std::string fitStringsToLine(const std::string & str1, const std::string & str2, char prefix = ' ');
    void printMenu(const std::vector<std::string> &msgs);
    void printMenu(const std::vector<std::string> &msgs, uint8_t possition, const std::vector<std::string> & add_msg);
    void printMenu(const std::vector<std::string> &msgs, uint8_t possition);

    void welcomeScreen();
    void mainMenuAction(const config::Key& key);
    void tempSensorsAction(const config::Key& key);
    void acLowRelaysAction(const config::Key& key);
    void acHighRelaysAction(const config::Key& key);
    void dcAcRelaysAction(const config::Key& key);
    void setAlarmAction(const config::Key& key);

    DisplayView current_view_;
    sensors::Ds18b20Collection& ds_collection_;
    io::Logger logger_;
};
}

#endif
