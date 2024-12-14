#include "display/lcd_hd_44780/LcdHd44780I2C.hpp"
#include <algorithm>
#include <config/AlarmWithNotification.hpp>
#include <config/RelaysAcHigh.hpp>
#include <config/RelaysAcLow.hpp>
#include <config/RelaysDcAc.hpp>
#include <display/Display.hpp>
#include <io/FunctionTimer.hpp>
#include <sensors/Ds18b20.hpp>

#include <algorithm>
#include <iomanip>
#include <stdexcept>

namespace display
{
    DisplayElementPosition::DisplayElementPosition() : row(0), col(0)
    {
    }

    DisplayElementPosition::DisplayElementPosition(uint8_t row, uint8_t col) : row(row), col(col)
    {
    }

    Display::Display(LcdHd44780I2C::Config lcdDisplayConfig) : lcdDisplay_(lcdDisplayConfig), logger_("Display")
    {
        if (!lcdDisplay_.init())
        {
            const std::string msg("Can't initialize display");
            logger_.error() << msg;
            throw std::runtime_error(msg);
        }
    }

    uint8_t Display::getMaxRows() const
    {
        return lcdDisplay_.getRows();
    }

    uint8_t Display::getMaxColumns() const
    {
        return lcdDisplay_.getColumns();
    }

    Display &Display::clearScreen()
    {
        elemCollection_.clear();
        lcdDisplay_.writeCommand(DisplayClearCommand());
        return *this;
    }

    Display &Display::nextLine()
    {
        if (cursor_.row < (lcdDisplay_.getRows() - 1))
        {
            setCursor(cursor_.row + 1, 0);
        }
        return *this;
    }

    void Display::setCursor(uint8_t row, uint8_t col)
    {
        cursor_ = {row, col};
        lcdDisplay_.setCursorPosition(row, col);
    }

    void Display::setCursor(const DisplayElementPosition &pos)
    {
        setCursor(pos.row, pos.col);
    }

    Display &Display::moveCursor(size_t pos)
    {
        const uint8_t maxCol = lcdDisplay_.getColumns();
        const uint8_t maxRow = lcdDisplay_.getRows();

        pos += cursor_.row * maxCol + cursor_.col;

        if (pos >= maxCol * maxRow)
        {
            setCursor(lcdDisplay_.getRows() - 1, lcdDisplay_.getColumns() - 1);
        }
        else
        {
            setCursor(pos / maxCol, pos % maxCol);
        }

        return *this;
    }

    Display &Display::addElement(const std::shared_ptr<DisplayElement> &element)
    {
        if (element->getPosition() != cursor_)
        {
            cursor_ = element->getPosition();
        }

        switch (element->getType())
        {
            case DisplayElementType::TEXT:
            {
                auto textElement = std::dynamic_pointer_cast<TextElement>(element);
                if (textElement)
                {
                    elemCollection_.push_back(element);
                    moveCursor(textElement->getTextLength());
                }
                else
                {
                    logger_.error() << "Can't cast DisplayElement to TextElement";
                }
                break;
            }
            case DisplayElementType::ANIMATED_TEXT:
            {
                auto animatedTextElement = std::dynamic_pointer_cast<AnimatedTextElement>(element);
                if (animatedTextElement)
                {
                    elemCollection_.push_back(element);
                    moveCursor(animatedTextElement->getTextLength());
                }
                else
                {
                    logger_.error() << "Can't cast DisplayElement to AnimatedTextElement";
                }
                break;
            }
            case DisplayElementType::WRAPPED_TEXT:
            {
                auto wrappedTextElement = std::dynamic_pointer_cast<WrappedTextElement>(element);
                if (wrappedTextElement)
                {
                    elemCollection_.push_back(element);
                    moveCursor(wrappedTextElement->getUnwrappedTextLength());
                }
                else
                {
                    logger_.error() << "Can't cast DisplayElement to WrappedTextElement";
                }
                break;
            }
            default:
            {
                logger_.error() << "Can't add DisplayElement - type is not supported";
                break;
            }
        }

        return *this;
    }

    Display &Display::addTextWithAnimating(const std::string &text, uint32_t animation_time)
    {
        elemCollection_.emplace_back(std::make_shared<AnimatedTextElement>(text, animation_time, cursor_));
        moveCursor(text.size());
        return *this;
    }

    Display &Display::addTextWithWrapping(const std::string &text, uint8_t unwrapped_part_length)
    {
        elemCollection_.emplace_back(std::make_shared<WrappedTextElement>(text, unwrapped_part_length, cursor_));
        moveCursor(unwrapped_part_length);
        return *this;
    }

    Display &Display::addText(const std::string &text)
    {
        elemCollection_.emplace_back(std::make_shared<TextElement>(text, cursor_));
        moveCursor(text.size());
        return *this;
    }

    void Display::show()
    {
        for (const auto &el: elemCollection_) { el->draw(lcdDisplay_); }
    }

}// namespace display
