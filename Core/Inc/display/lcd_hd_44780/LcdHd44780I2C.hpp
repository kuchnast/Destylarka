#ifndef DISPLAY_LCD_HD44780_HPP
#define DISPLAY_LCD_HD44780_HPP

#include "LcdHd44780Commands.hpp"

#include "stm32f4xx_hal.h"
#include <array>
#include <string>

namespace display
{

    class LcdHd44780I2C
    {
    public:
        struct Config
        {
            I2C_HandleTypeDef *i2c;
            uint8_t address;
            uint8_t lines;
            uint8_t columns;
        };

        explicit LcdHd44780I2C(const Config &config);

        LcdHd44780I2C(const LcdHd44780I2C &) = delete;

        LcdHd44780I2C &operator=(const LcdHd44780I2C &) = delete;

        bool init();

        bool writeCommand(LcdHd44780Command &&command);

        bool setBacklight(bool state);

        bool setCursorPosition(uint8_t line, uint8_t column);

        uint8_t getRows() const;

        uint8_t getColumns() const;

        LcdHd44780I2C &operator<<(char ch);

        LcdHd44780I2C &operator<<(const std::string &str);

        LcdHd44780I2C &operator<<(std::string &&str);

    private:
        enum LcdBit : uint8_t
        {
            LCD_BIT_RS = 0x01U,
            LCD_BIT_RW = 0x02U,
            LCD_BIT_E = 0x04U,
            LCD_BIT_BACKLIGHT = 0x08U,
            LCD_BIT_DB4 = 0x10U,
            LCD_BIT_DB5 = 0x20U,
            LCD_BIT_DB6 = 0x40U,
            LCD_BIT_DB7 = 0x80U,
        };

        static constexpr std::array<uint8_t, 4> LINE_OFFSETS{0x00, 0x40, 0x14, 0x54};

        bool writeRaw(uint8_t byte, bool with_enable);

        const LcdHd44780I2C::Config config_{};
        bool backlight_;
    };

}// namespace display

#endif// DISPLAY_LCD_HD44780_HPP