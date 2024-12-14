#include "display/lcd_hd_44780/LcdHd44780Commands.hpp"

namespace display
{

    LcdHd44780Command::LcdHd44780Command() : signalRS_(false), signalRW_(false), data_(0) {}

    LcdHd44780Command::LcdHd44780Command(bool signalRS, bool signalRW, uint8_t data)
        : signalRS_(signalRS), signalRW_(signalRW), data_(data)
    {
    }

    uint16_t LcdHd44780Command::getCode() const
    {
        uint16_t code = data_;

        if (signalRW_) { code |= (0x01 << RW_BIT); }
        if (signalRS_) { code |= (0x01 << RS_BIT); }

        return code;
    }

    uint8_t LcdHd44780Command::getData() const { return data_; }

    bool LcdHd44780Command::getSignalRS() const { return signalRS_; }

    bool LcdHd44780Command::getSignalRW() const { return signalRW_; }

    void LcdHd44780Command::setDataBit(uint8_t bit_num, bool state)
    {
        if (state) { data_ |= (0x01 << bit_num); }
        else { data_ &= ~(0x01 << bit_num); }
    }

    void LcdHd44780Command::setData(uint8_t data) { data_ = data; }

    void LcdHd44780Command::setSignalRS(bool state) { signalRS_ = state; }

    void LcdHd44780Command::setSignalRW(bool state) { signalRW_ = state; }

    DisplayClearCommand::DisplayClearCommand() : LcdHd44780Command(false, false, DISPLAY_CLEAR) {}

    CursorReturnHomeCommand::CursorReturnHomeCommand() : LcdHd44780Command(false, false, CURSOR_RETURN_HOME) {}

    EntryModeControlCommand::EntryModeControlCommand(DirectionControl direction, DisplayControl display)
        : LcdHd44780Command(false, false, ENTRY_MODE | toUnderlying(direction) | toUnderlying(display))
    {
    }

    DisplayControlCommand::DisplayControlCommand(DISPLAY display, CURSOR cursor, BLINK blink)
        : LcdHd44780Command(false, false,
              DISPLAY_CONTROL | toUnderlying(display) | toUnderlying(cursor) | toUnderlying(blink))
    {
    }

    CursorOrDisplayShiftCommand::CursorOrDisplayShiftCommand(ShiftDirection direction, ShiftMode mode)
        : LcdHd44780Command(false, false, CURSOR_OR_DISPLAY_SHIFT | toUnderlying(direction) | toUnderlying(mode))
    {
    }

    FunctionControlCommand::FunctionControlCommand(BitMode bit_mode, LineMode line_mode, FontMode font_mode)
        : LcdHd44780Command(false, false,
              FUNCTION_SET | toUnderlying(bit_mode) | toUnderlying(line_mode) | toUnderlying(font_mode))
    {
    }

    SetCGRAMCommand::SetCGRAMCommand(uint8_t cell, bool is_5x8_font)
        : LcdHd44780Command(false, false,
              SET_CGRAM_ADDR | (is_5x8_font ? (cell << CELL_FONT_5X8_SHIFT) : (cell << CELL_FONT_5X10_SHIFT)))
    {
    }

    SetDDRAMCommand::SetDDRAMCommand(uint8_t address) : LcdHd44780Command(false, false, SET_DDRAM_ADDR | address) {}

    ReadBusyFlagCommand::ReadBusyFlagCommand() : LcdHd44780Command(false, true, 0) {}

    WriteDataCommand::WriteDataCommand(uint8_t data) : LcdHd44780Command(true, false, data) {}

    ReadDataCommand::ReadDataCommand() : LcdHd44780Command(true, true, 0) {}

}// namespace display
