#ifndef	CONFIG_KEYPAD_HPP_
#define	CONFIG_KEYPAD_HPP_

#include <io/GpioPin.hpp>
#include <main.hpp>

#include <algorithm>
#include <array>
#include <string>
#include <map>

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

    static const std::map<Key, std::string> key_string_map = {
            {Key::NONE, "NONE"},
            {Key::F1, "F1"},
            {Key::F2, "F2"},
            {Key::HASH, "#"},
            {Key::STAR, "*"},
            {Key::ARROW_UP, "ARROW_UP"},
            {Key::ARROW_DOWN, "ARROW_DOWN"},
            {Key::ARROW_LEFT, "ARROW_LEFT"},
            {Key::ARROW_RIGHT, "ARROW_RIGHT"},
            {Key::ESC, "ESC"},
            {Key::ENTER, "ENTER"},
            {Key::N0, "0"},
            {Key::N1, "1"},
            {Key::N2, "2"},
            {Key::N3, "3"},
            {Key::N4, "4"},
            {Key::N5, "5"},
            {Key::N6, "6"},
            {Key::N7, "7"},
            {Key::N8, "8"},
            {Key::N9, "9"}};

    static std::string toString(const Key &key)
    {
        auto it = key_string_map.find(key);
        if (it == key_string_map.end())
            return "UNKNOWN";

        return it->second;
    }

    static Key toKey(std::string key_name)
    {
        std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::toupper);

        auto result = std::find_if(key_string_map.begin(), key_string_map.end(), [&key_name](
            const auto& el) {return el.second == key_name;});

        return result == key_string_map.end() ? Key::NONE : result->first;
    }

} //  namespace config

#endif //  CONFIG_KEYPAD_HPP_
