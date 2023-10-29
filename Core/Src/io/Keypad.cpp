#include <config/Keypad.hpp>
#include <io/Keypad.hpp>

#include <gpio.h>

#include <cstdio>

namespace io {

    Keypad::Keypad() : column_size_(config::keypad_column.size()), row_size_(config::keypad_row.size()) {}

    uint16_t Keypad::scanKeys()
    {
        uint16_t key = 0;

        for (auto & col_pin : config::keypad_column)
        {
            for (auto & pin : config::keypad_column)
                pin.write(PinState::SET);

            col_pin.write(PinState::RESET);
            HAL_Delay(5);

            for (auto & row_pin : config::keypad_row) {
                if (row_pin.read() == PinState::RESET) {
                    HAL_Delay(config::keypad_debounce_time_ms);
                    if (row_pin.read() == PinState::RESET)
                    {
                        key |= 1 << c;
                        key |= 1 << (r + 8);
                        while (row_pin.read() == PinState::RESET)
                            HAL_Delay(5);
                        return key;
                    }
                }
            }
        }
        return key;
    }
    //#############################################################################################
    uint16_t KeyPad_WaitForKeyGetUint(uint32_t Timeout_ms) {
        uint16_t keyRead;
        while (Timeout_ms == 0) {
            keyRead = KeyPad_Scan();
            if (keyRead != 0) {
                KeyPad.LastKey = keyRead;
                return keyRead;
            }
            HAL_Delay(_KEYPAD_DEBOUNCE_TIME_MS);
        }

        uint32_t StartTime = HAL_GetTick();
        while (HAL_GetTick() - StartTime < Timeout_ms) {
            keyRead = KeyPad_Scan();
            if (keyRead != 0) {
                KeyPad.LastKey = keyRead;
                return keyRead;
            }
            HAL_Delay(_KEYPAD_DEBOUNCE_TIME_MS);
        }
        KeyPad.LastKey = 0;
        return 0;
    }

    config::Key Keypad::waitForKey(uint32_t timeout_ms) {
        uint16_t i = KeyPad_WaitForKeyGetUint(Timeout_ms);
        switch (i) {
            case 0x0000:
                return K_NONE;
                break;
            case 0x1001:
                return K_F1;
                break;
            case 0x1002:
                return K_F2;
                break;
            case 0x1004:
                return K_HASH;
                break;
            case 0x1008:
                return K_STAR;
                break;
            case 0x0801:
                return K_1;
                break;
            case 0x0802:
                return K_2;
                break;
            case 0x0804:
                return K_3;
                break;
            case 0x0808:
                return K_ARROW_UP;
                break;
            case 0x0401:
                return K_4;
                break;
            case 0x0402:
                return K_5;
                break;
            case 0x0404:
                return K_6;
                break;
            case 0x0408:
                return K_ARROW_DOWN;
                break;
            case 0x0201:
                return K_7;
                break;
            case 0x0202:
                return K_8;
                break;
            case 0x0204:
                return K_9;
                break;
            case 0x0208:
                return K_ESC;
                break;
            case 0x0101:
                return K_ARROW_LEFT;
                break;
            case 0x0102:
                return K_0;
                break;
            case 0x0104:
                return K_ARROW_RIGHT;
                break;
            case 0x0108:
                return K_ENTER;
                break;
            default:
                return K_NONE;
                break;
        }
    }
}
