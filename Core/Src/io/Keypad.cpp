#include <config/Keypad.hpp>
#include <io/Keypad.hpp>

#include <gpio.h>

#include <cstdio>

namespace io {

    using namespace config;

    Keypad::Keypad() : column_size_(keypad_column.size()), row_size_(keypad_row.size()) {}

    uint16_t Keypad::scanKeys()
    {
        uint16_t key = 0;

        for (uint8_t c = 0; c < keypad_column.size(); ++c)
        {
            for (auto &pin: keypad_column)
                pin.write(PinState::SET);

            keypad_column[c].write(PinState::RESET);
            HAL_Delay(5);

            for (uint8_t r = 0; r < keypad_row.size(); ++r) {
                if (keypad_row[r].read() == PinState::RESET) {
                    HAL_Delay(keypad_debounce_time_ms);
                    if (keypad_row[r].read() == PinState::RESET) {
                        key |= 1 << c;
                        key |= 1 << (r + 8);
                        while (keypad_row[r].read() == PinState::RESET)
                            HAL_Delay(5);
                        return key;
                    }
                }
            }
        }

        return key;
    }

    Key Keypad::waitForKey(uint32_t timeout_ms)
    {
    	uint16_t  keyRead = 0;

    	while(timeout_ms==0)
    	{
    		keyRead = scanKeys();
    		if(keyRead != 0)
    		{
    			break;
    		}
    		HAL_Delay(keypad_debounce_time_ms);
    	}

    	uint32_t	StartTime = HAL_GetTick();
    	while(HAL_GetTick()-StartTime < timeout_ms)
    	{
    		keyRead = scanKeys();
    		if(keyRead != 0)
    		{
    			break;
    		}
            HAL_Delay(keypad_debounce_time_ms);
    	}

        switch (keyRead)
        {
            case 0x0000:
                return Key::NONE;
            case 0x1001:
                return Key::F1;
            case 0x1002:
                return Key::F2;
            case 0x1004:
                return Key::HASH;
            case 0x1008:
                return Key::STAR;
            case 0x0801:
                return Key::N1;
            case 0x0802:
                return Key::N2;
            case 0x0804:
                return Key::N3;
            case 0x0808:
                return Key::ARROW_UP;
            case 0x0401:
                return Key::N4;
            case 0x0402:
                return Key::N5;
            case 0x0404:
                return Key::N6;
            case 0x0408:
                return Key::ARROW_DOWN;
            case 0x0201:
                return Key::N7;
            case 0x0202:
                return Key::N8;
            case 0x0204:
                return Key::N9;
            case 0x0208:
                return Key::ESC;
            case 0x0101:
                return Key::ARROW_LEFT;
            case 0x0102:
                return Key::N0;
            case 0x0104:
                return Key::ARROW_RIGHT;
            case 0x0108:
                return Key::ENTER;
            default:
                return Key::NONE;
        }
    }
}
