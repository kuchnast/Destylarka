#ifndef DISPLAY_DISPLAY_HPP
#define DISPLAY_DISPLAY_HPP

#include <algorithm>
#include <config/CustomDisplayCharacters.hpp>
#include <display/lcd_hd_44780/LcdHd44780I2C.hpp>
#include <io/Logger.hpp>

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace display
{
    struct DisplayElementPosition
    {
        uint8_t row;
        uint8_t col;

        DisplayElementPosition();
        DisplayElementPosition(uint8_t row, uint8_t col);

        bool operator==(const DisplayElementPosition &other) const
        {
            return (row == other.row) && (col == other.col);
        }

        bool operator!=(const DisplayElementPosition &other) const
        {
            return !(*this == other);
        }
    };

    enum class DisplayElementType : uint8_t
    {
        TEXT,
        ANIMATED_TEXT,
        WRAPPED_TEXT
    };

    class DisplayElement
    {
    public:
        DisplayElement() = delete;
        virtual ~DisplayElement() = default;
        DisplayElement(const DisplayElementPosition &pos, DisplayElementType type) : pos_(pos), type_(type)
        {
        }

        virtual void draw(LcdHd44780I2C &lcdDisplay) = 0;

        DisplayElementType getType() const
        {
            return type_;
        }

        DisplayElementPosition getPosition() const
        {
            return pos_;
        }

    private:
        DisplayElementPosition pos_;
        const DisplayElementType type_;
    };

    using DisplayElementsCollection = std::vector<std::shared_ptr<DisplayElement>>;

    class TextElement : public DisplayElement
    {
    public:
        TextElement() = delete;
        TextElement(std::string text, const DisplayElementPosition &pos)
            : text_(std::move(text)), DisplayElement(pos, DisplayElementType::TEXT)
        {
        }

        void draw(LcdHd44780I2C &lcdDisplay) override
        {
            auto cursor = getPosition();
            lcdDisplay.setCursorPosition(cursor.row, cursor.col);
            lcdDisplay << text_;
        }

        std::basic_string<char>::size_type getTextLength()
        {
            return text_.size();
        }

        const std::string &getText()
        {
            return text_;
        }

    private:
        std::string text_;
    };

    class AnimatedTextElement : public DisplayElement
    {
    public:
        AnimatedTextElement() = delete;
        AnimatedTextElement(std::string text, uint32_t animation_time_ms, const DisplayElementPosition &pos)
            : text_(std::move(text)),
              animation_time_ms_(animation_time_ms < text.size() ? text.size() : animation_time_ms),
              DisplayElement(pos, DisplayElementType::ANIMATED_TEXT)
        {
        }

        void draw(LcdHd44780I2C &lcdDisplay) override
        {
            if (!background_task_id_)
            {
                auto fun = [&lcdDisplay, this]()
                {
                    static std::basic_string<char>::size_type pos = 0;
                    if (pos < text_.size())
                    {
                        auto cursor = this->getPosition();
                        lcdDisplay.setCursorPosition(cursor.row, cursor.col + pos);
                        lcdDisplay << text_[pos++];
                    }
                    else
                    {
                        io::FunctionTimer::modifyFunctionRepeatability(background_task_id_.value(), false);
                    }
                };
                background_task_id_ = io::FunctionTimer::addFunction(fun, {animation_time_ms_ / text_.size()},
                    "AnimatedTextElement", true);
            }
        }

        ~AnimatedTextElement() override
        {
            if (background_task_id_)
            {
                io::FunctionTimer::removeFunction(background_task_id_.value());
            }
        }

        std::basic_string<char>::size_type getTextLength()
        {
            return text_.size();
        }

    private:
        std::string text_;
        uint32_t animation_time_ms_;
        std::optional<uint32_t> background_task_id_;
    };

    class WrappedTextElement : public DisplayElement
    {
    public:
        WrappedTextElement() = delete;
        WrappedTextElement(const std::string &text, uint8_t unwrapped_part_length, const DisplayElementPosition &pos)
            : text_(text),
              unwrapped_text_length_(std::min<std::basic_string<char>::size_type>(unwrapped_part_length, text.size())),
              current_part_pos_(0), DisplayElement(pos, DisplayElementType::WRAPPED_TEXT)
        {
        }

        void draw(LcdHd44780I2C &lcdDisplay) override
        {
            auto cursor = getPosition();
            lcdDisplay.setCursorPosition(cursor.row, cursor.col);

            auto available_length = unwrapped_text_length_;
            bool show_left_arrow = current_part_pos_ > 0;
            bool show_right_arrow = current_part_pos_ + available_length < text_.size();

            if (show_left_arrow)
                --available_length;
            if (show_right_arrow)
                --available_length;

            auto substr_length =
                std::min<std::basic_string<char>::size_type>(available_length, text_.size() - current_part_pos_);
            auto substr = text_.substr(current_part_pos_, substr_length);

            if (show_left_arrow)
                lcdDisplay << '<';
            lcdDisplay << substr;
            if (show_right_arrow)
                lcdDisplay << '>';
        }

        void nextPart()
        {
            if (current_part_pos_ + unwrapped_text_length_ < text_.size())
            {
                ++current_part_pos_;
            }
        }

        void prevPart()
        {
            if (current_part_pos_ > 0)
            {
                --current_part_pos_;
            }
        }

        bool isLastPart() const
        {
            return current_part_pos_ + unwrapped_text_length_ >= text_.size();
        }

        bool isFirstPart() const
        {
            return current_part_pos_ == 0;
        }

        std::basic_string<char>::size_type getTextLength() const
        {
            return text_.size();
        }

        std::basic_string<char>::size_type getUnwrappedTextLength() const
        {
            return unwrapped_text_length_;
        }

    private:
        std::string text_;
        std::basic_string<char>::size_type unwrapped_text_length_;
        std::basic_string<char>::size_type current_part_pos_;
    };


    class Display
    {
    public:
        explicit Display(LcdHd44780I2C::Config lcdDisplayConfig);

        uint8_t getMaxRows() const;
        uint8_t getMaxColumns() const;

        Display &clearScreen();
        Display &nextLine();
        Display &moveCursor(size_t pos);
        Display &addElement(const std::shared_ptr<DisplayElement> &element);
        Display &addTextWithAnimating(const std::string &text, uint32_t animation_time);
        Display &addTextWithWrapping(const std::string &text, uint8_t unwrapped_part_length);
        Display &addText(const std::string &text);
        void show();

    private:
        void setCursor(uint8_t row, uint8_t col);
        void setCursor(const DisplayElementPosition &pos);

        LcdHd44780I2C lcdDisplay_;
        DisplayElementPosition cursor_;
        DisplayElementsCollection elemCollection_;

        io::Logger logger_;
    };


}// namespace display

#endif// DISPLAY_DISPLAY_HPP