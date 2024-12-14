#ifndef DISPLAY_VIEW_MANAGER_HPP
#define DISPLAY_VIEW_MANAGER_HPP

#include "config/Keypad.hpp"
#include <display/Display.hpp>
#include <io/Keypad.hpp>
#include <io/Logger.hpp>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace display
{
    enum class View
    {
        EMPTY,
        MAIN_MENU,
        TEMP_SENSORS,
        AC_LOW_RELAYS,
        AC_HIGH_RELAYS,
        DC_AC_RELAYS,
        SET_ALARM,
        ALARM_NOTIFICATION
    };
    std::string toString(const View &view);

    class DisplayViewManager
    {
    public:
        explicit DisplayViewManager(const LcdHd44780I2C::Config &lcdDisplayConfig);
        static void setView(View view);
        static View getCurrentView();
        void handleKeyInput(config::Key key);

    private:
        Display display_;
        static View current_view_;
        io::Logger logger_;
    };
}// namespace display

#endif// DISPLAY_VIEW_MANAGER_HPP
