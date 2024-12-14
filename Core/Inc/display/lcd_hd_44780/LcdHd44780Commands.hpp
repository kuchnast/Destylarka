#ifndef DISPLAY_LCD_HD44780_COMMANDS_HPP
#define DISPLAY_LCD_HD44780_COMMANDS_HPP

#include <cstdint>
#include <type_traits>

namespace display
{

    template<typename E>
    constexpr std::underlying_type_t<E> toUnderlying(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }

    enum class LcdHd44780Commands : uint8_t
    {
        DISPLAY_CLEAR,
        CURSOR_RETURN_HOME,
        ENTRY_MODE_CONTROL,
        DISPLAY_CONTROL,
        CURSOR_OR_DISPLAY_SHIFT,
        FUNCTION_CONTROL,
        SET_CGRAM_ADDR,
        SET_DDRAM_ADDR,
        READ_BUSY_FLAG,
        WRITE_DATA,
        READ_DATA
    };

    class LcdHd44780Command
    {
    public:
        LcdHd44780Command();
        LcdHd44780Command(bool signalRS, bool signalRW, uint8_t data);
        uint16_t getCode() const;
        uint8_t getData() const;
        bool getSignalRS() const;
        bool getSignalRW() const;

    protected:
        void setDataBit(uint8_t bit_num, bool state);
        void setData(uint8_t data);
        void setSignalRS(bool state);
        void setSignalRW(bool state);

    private:
        static constexpr uint8_t RW_BIT = 8;
        static constexpr uint8_t RS_BIT = 9;

        bool signalRS_;
        bool signalRW_;
        uint8_t data_;
    };

    class DisplayClearCommand : public LcdHd44780Command
    {
    public:
        DisplayClearCommand();

    private:
        static constexpr uint8_t DISPLAY_CLEAR = 0x01U;
    };

    class CursorReturnHomeCommand : public LcdHd44780Command
    {
    public:
        CursorReturnHomeCommand();

    private:
        static constexpr uint8_t CURSOR_RETURN_HOME = 0x02U;
    };

    class EntryModeControlCommand : public LcdHd44780Command
    {
    public:
        enum DirectionControl : uint8_t
        {
            DIRECTION_LEFT = 0x00U,
            DIRECTION_RIGHT = 0x02U,
        };

        enum DisplayControl : uint8_t
        {
            DISPLAY_NO_SHIFT = 0x00U,
            DISPLAY_SHIFT = 0x01U,
        };

        EntryModeControlCommand(DirectionControl direction, DisplayControl display);

    private:
        static constexpr uint8_t ENTRY_MODE = 0x04U;
    };

    class DisplayControlCommand : public LcdHd44780Command
    {
    public:
        enum DISPLAY : uint8_t
        {
            DISPLAY_OFF = 0x00U,
            DISPLAY_ON = 0x04U,
        };

        enum CURSOR : uint8_t
        {
            CURSOR_OFF = 0x00U,
            CURSOR_ON = 0x02U,
        };

        enum BLINK : uint8_t
        {
            BLINK_OFF = 0x00U,
            BLINK_ON = 0x01U,
        };

        DisplayControlCommand(DISPLAY display, CURSOR cursor, BLINK blink);

    private:
        static constexpr uint8_t DISPLAY_CONTROL = 0x08U;
    };

    class CursorOrDisplayShiftCommand : public LcdHd44780Command
    {
    public:
        enum ShiftMode : uint8_t
        {
            DISPLAY_SHIFT = 0x00U,
            CURSOR_SHIFT = 0x08U,
        };

        enum ShiftDirection : uint8_t
        {
            DIRECTION_LEFT = 0x00U,
            DIRECTION_RIGHT = 0x04U,
        };

        CursorOrDisplayShiftCommand(ShiftDirection direction, ShiftMode mode);

    private:
        static constexpr uint8_t CURSOR_OR_DISPLAY_SHIFT = 0x10U;
    };

    class FunctionControlCommand : public LcdHd44780Command
    {
    public:
        enum BitMode : uint8_t
        {
            MODE_4_BIT = 0x00U,
            MODE_8_BIT = 0x10U,
        };

        enum LineMode : uint8_t
        {
            MODE_1_LINE = 0x00U,
            MODE_2_LINES = 0x08U,
        };

        enum FontMode : uint8_t
        {
            FONT_5X8_DOTS = 0x00U,
            FONT_5X10_DOTS = 0x04U,
        };

        FunctionControlCommand(BitMode bit_mode, LineMode line_mode, FontMode font_mode);

    private:
        static constexpr uint8_t FUNCTION_SET = 0x20U;
    };


    class SetCGRAMCommand : public LcdHd44780Command
    {
    public:
        SetCGRAMCommand(uint8_t cell, bool is_5x8_font);

    private:
        static constexpr uint8_t SET_CGRAM_ADDR = 0x40U;
        static constexpr uint8_t CELL_FONT_5X8_SHIFT = 3;
        static constexpr uint8_t CELL_FONT_5X10_SHIFT = 4;
    };

    class SetDDRAMCommand : public LcdHd44780Command
    {
    public:
        explicit SetDDRAMCommand(uint8_t address);

    private:
        static constexpr uint8_t SET_DDRAM_ADDR = 0x80U;
    };

    class ReadBusyFlagCommand : public LcdHd44780Command
    {
    public:
        ReadBusyFlagCommand();
    };

    class WriteDataCommand : public LcdHd44780Command
    {
    public:
        explicit WriteDataCommand(uint8_t data);
    };

    class ReadDataCommand : public LcdHd44780Command
    {
    public:
        ReadDataCommand();
    };


}// namespace display

#endif// DISPLAY_LCD_HD44780_COMMANDS_HPP