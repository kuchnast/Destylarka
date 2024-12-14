#include "display/lcd_hd_44780/LcdHd44780I2C.hpp"
#include "stm32f4xx_hal_i2c.h"

#include <algorithm>
#include <array>

namespace display
{

    LcdHd44780I2C::LcdHd44780I2C(const Config &config) : config_(config), backlight_(false)
    {
    }

    bool LcdHd44780I2C::init()
    {
        constexpr static uint8_t firstDelayMs = 5;
        constexpr static uint8_t nextDelayMs = 1;

        bool state = true;

        // init sequence
        state &= writeRaw(LCD_BIT_DB5 | LCD_BIT_DB4, true);
        HAL_Delay(firstDelayMs);
        state &= writeRaw(LCD_BIT_DB5 | LCD_BIT_DB4, true);
        HAL_Delay(nextDelayMs);
        state &= writeRaw(LCD_BIT_DB5 | LCD_BIT_DB4, true);
        HAL_Delay(nextDelayMs);

        // set 4-bit mode
        state &= writeRaw(LCD_BIT_DB5, true);
        HAL_Delay(nextDelayMs);

        // set lines number and font
        state &= writeCommand(FunctionControlCommand(FunctionControlCommand::MODE_4_BIT,
            (config_.lines > 1 ? FunctionControlCommand::MODE_2_LINES : FunctionControlCommand::MODE_1_LINE),
            FunctionControlCommand::FONT_5X8_DOTS));

        setBacklight(true);
        state &= writeCommand(DisplayControlCommand(DisplayControlCommand::DISPLAY_ON, DisplayControlCommand::CURSOR_ON,
            DisplayControlCommand::BLINK_ON));
        state &= writeCommand(EntryModeControlCommand(EntryModeControlCommand::DIRECTION_RIGHT,
            EntryModeControlCommand::DISPLAY_NO_SHIFT));
        state &= writeCommand(DisplayClearCommand());

        return state;
    }

    bool LcdHd44780I2C::setBacklight(bool state)
    {
        backlight_ = state;
        return writeRaw((backlight_ ? LCD_BIT_BACKLIGHT : 0), false);
    }

    bool LcdHd44780I2C::setCursorPosition(uint8_t line, uint8_t column)
    {
        if (line >= config_.lines)
        {
            line = config_.lines - 1;
        }

        if (column >= config_.columns)
        {
            column = config_.columns - 1;
        }

        return writeCommand(SetDDRAMCommand(column | LINE_OFFSETS[line]));
    }

    uint8_t LcdHd44780I2C::getRows() const
    {
        return config_.lines;
    }

    uint8_t LcdHd44780I2C::getColumns() const
    {
        return config_.columns;
    }

    LcdHd44780I2C &LcdHd44780I2C::operator<<(char ch)
    {
        writeCommand(WriteDataCommand(ch));
        return *this;
    }

    LcdHd44780I2C &LcdHd44780I2C::operator<<(const std::string &str)
    {
        std::for_each(str.begin(), str.end(), [this](char ch) { *this << ch; });
        return *this;
    }

    LcdHd44780I2C &LcdHd44780I2C::operator<<(std::string &&str)
    {
        std::for_each(str.begin(), str.end(), [this](char ch) { *this << ch; });
        return *this;
    }

    bool LcdHd44780I2C::writeCommand(LcdHd44780Command &&command)
    {
        static constexpr uint8_t mask4Bit = 0xF0;
        std::array<uint8_t, 4> lcdCommandBuffer;
        const uint8_t data = command.getData();

        std::fill(lcdCommandBuffer.begin(), lcdCommandBuffer.end(),
            (command.getSignalRS() ? LCD_BIT_RS : 0) | (command.getSignalRW() ? LCD_BIT_RW : 0) |
                (backlight_ ? LCD_BIT_BACKLIGHT : 0));

        lcdCommandBuffer[0] |= LCD_BIT_E | (data & mask4Bit);
        lcdCommandBuffer[1] |= (data & mask4Bit);
        lcdCommandBuffer[2] |= LCD_BIT_E | ((data << 4) & mask4Bit);
        lcdCommandBuffer[3] |= ((data << 4) & mask4Bit);

        return HAL_I2C_Master_Transmit(config_.i2c, config_.address << 1, lcdCommandBuffer.data(),
                   lcdCommandBuffer.size(), 100) == HAL_OK;
    }

    bool LcdHd44780I2C::writeRaw(uint8_t byte, bool with_enable)
    {
        if (with_enable)
        {
            std::array<uint8_t, 2> lcdCommandBuffer = {static_cast<uint8_t>(byte | LCD_BIT_E), byte};

            if (HAL_I2C_Master_Transmit(config_.i2c, config_.address << 1, lcdCommandBuffer.data(),
                    lcdCommandBuffer.size(), 100) != HAL_OK)
            {
                return false;
            }
        }
        else
        {
            if (HAL_I2C_Master_Transmit(config_.i2c, config_.address << 1, &byte, sizeof(byte), 100) != HAL_OK)
            {
                return false;
            }
        }
        return true;
    }

}// namespace display