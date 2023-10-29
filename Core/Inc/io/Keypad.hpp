#ifndef	IO_KEYPAD_HPP_
#define	IO_KEYPAD_HPP_

#include "config/Keypad.hpp"

namespace io
{

    class Keypad
    {
    public:
        Keypad();
        config::Key waitForKey(uint32_t timeout_ms);

    private:
        uint16_t scanKeys();

        const uint8_t column_size_;
        const uint8_t row_size_;
    };

} //  namespace io

#endif //  IO_KEYPAD_HPP_
