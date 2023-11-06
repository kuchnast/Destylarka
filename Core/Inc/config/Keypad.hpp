#ifndef	CONFIG_KEYPAD_HPP_
#define	CONFIG_KEYPAD_HPP_

#include <io/GpioPin.hpp>
#include <main.hpp>

#include <array>
#include <string>

namespace config {

    constexpr uint32_t keypad_debounce_time_ms = 20;

    static std::array<io::GpioPin, 4> keypad_column{
        io::GpioPin{K_COL_1_GPIO_Port, K_COL_1_Pin},
        io::GpioPin{K_COL_2_GPIO_Port, K_COL_2_Pin},
        io::GpioPin{K_COL_3_GPIO_Port, K_COL_3_Pin},
        io::GpioPin{K_COL_4_GPIO_Port, K_COL_4_Pin}};

    static std::array<io::GpioPin, 5> keypad_row{
        io::GpioPin{K_ROW_1_GPIO_Port, K_ROW_1_Pin},
        io::GpioPin{K_ROW_2_GPIO_Port, K_ROW_2_Pin},
        io::GpioPin{K_ROW_3_GPIO_Port, K_ROW_3_Pin},
        io::GpioPin{K_ROW_4_GPIO_Port, K_ROW_4_Pin},
        io::GpioPin{K_ROW_5_GPIO_Port, K_ROW_5_Pin}};


    enum class Key
    {
        NONE = 0,
        F1,
        F2,
        HASH,
        STAR,
        ARROW_UP,
        ARROW_DOWN,
        ARROW_LEFT,
        ARROW_RIGHT,
        ESC,
        ENTER,
        N0,
        N1,
        N2,
        N3,
        N4,
        N5,
        N6,
        N7,
        N8,
        N9,
    };

    static std::string toString(const Key &key)
    {
        switch(key)
        {
            case Key::NONE:
                return "NONE";
            case Key::F1:
                return "F1";
            case Key::F2:
                return "F2";
            case Key::HASH:
                return "#";
            case Key::STAR:
                return "*";
            case Key::ARROW_UP:
                return "ARROW_UP";
            case Key::ARROW_DOWN:
                return "ARROW_DOWN";
            case Key::ARROW_LEFT:
                return "ARROW_LEFT";
            case Key::ARROW_RIGHT:
                return "ARROW_RIGHT";
            case Key::ESC:
                return "ESC";
            case Key::ENTER:
                return "ENTER";
            case Key::N0:
                return "0";
            case Key::N1:
                return "1";
            case Key::N2:
                return "2";
            case Key::N3:
                return "3";
            case Key::N4:
                return "4";
            case Key::N5:
                return "5";
            case Key::N6:
                return "6";
            case Key::N7:
                return "7";
            case Key::N8:
                return "8";
            case Key::N9:
                return "9";
            default:
                return "UNKNOWN";
        }
    }

} //  namespace config

#endif //  CONFIG_KEYPAD_HPP_
