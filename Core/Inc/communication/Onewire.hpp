#ifndef COMMUNICATION_ONEWIRE_H
#define COMMUNICATION_ONEWIRE_H

#include <io/GpioPin.hpp>

#include <tim.h>

#include <map>

namespace communication {

    using OneWireAddress = std::array<uint8_t, 8>;

    enum class OneWireCommand
    {
        R_SCRATCHPAD,
        W_SCRATCHPAD,
        CPY_SCRATCHPAD,
        REC_EEPROM,
        R_PWR_SUPPLY,
        SEARCH_ROM,
        READ_ROM,
        MATCH_ROM,
        SKIP_ROM
    };

    class OneWire {
    public:
        OneWire(io::GpioPin & pin, TIM_HandleTypeDef &tim);

        bool resetBus();

        void writeBit(bool bit);
        void writeByte(uint8_t byte);
        bool readBit();
        uint8_t readByte();

        void resetSearch();
        bool findDevice(bool is_first = false);
        uint8_t search(OneWireCommand commandId);

        OneWireAddress getDeviceAddress();
        void selectDevice(OneWireAddress& addr);

        uint8_t calculateCrc8(uint8_t *addr, uint8_t len);

    private:
        void delay(uint16_t us) const;
        void setBusInputDirection();
        void setBusOutputDirection();

        static std::map<OneWireCommand, uint8_t> commands_;

        TIM_HandleTypeDef &tim_;             // OneWire timer
        io::GpioPin & pin_;                  // Bus GPIO Pin
        uint8_t last_discrepancy_;           // For searching purpose
        uint8_t last_family_discrepancy_;    // For searching purpose
        uint8_t last_device_flag_;           // For searching purpose
        OneWireAddress rom_address_;         // 8-byte ROM addres last found device
    };
}

#endif

