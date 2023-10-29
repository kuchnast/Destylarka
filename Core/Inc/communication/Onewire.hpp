#ifndef COMMUNICATION_ONEWIRE_H
#define COMMUNICATION_ONEWIRE_H

#include "gpio.h"

namespace communication {

    enum class OneWireCommands
    {
        R_SCRATCHPAD = 0xBE,
        W_SCRATCHPAD = 0x4E,
        CPY_SCRATCHPAD = 0x48,
        REC_EEPROM = 0xB8,
        R_PWR_SUPPLY = 0xB4,
        SEARCH_ROM = 0xF0,
        READ_ROM = 0x33,
        MATCH_ROM = 0x55,
        SKIP_ROM = 0xCC
    };

    class OneWire {
    public:
        void init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, );

        uint8_t reset();

        void resetSearch();
        uint8_t first();
        uint8_t next();
        uint8_t search(uint8_t command);

        void writeBit(uint8_t bit);
        uint8_t readBit();
        void writeByte(uint8_t byte);
        uint8_t readByte();

        void getFullROM(uint8_t *firstIndex);
        void select(uint8_t *addr);
        void selectWithPointer(uint8_t *ROM);

        uint8_t calculateCRC8(uint8_t *addr, uint8_t len);

    private:
        GPIO_TypeDef *gpio_;             // Bus GPIO Port
        uint16_t pin_;                   // Bus GPIO Pin
        uint8_t last_discrepancy_;       // For searching purpose
        uint8_t last_family_discrepancy_;// For searching purpose
        uint8_t last_device_flag_;       // For searching purpose
        uint8_t address_[8];             // 8-byte ROM addres last found device
    };
}

#endif

