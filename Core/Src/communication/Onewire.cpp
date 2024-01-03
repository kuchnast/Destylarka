#include "communication/Onewire.hpp"
#include "sensors/Ds18b20.hpp"
#include "tim.h"

namespace communication {

    std::map<OneWireCommand, uint8_t> OneWire::commands{
        {OneWireCommand::R_SCRATCHPAD, 0xBE},
        {OneWireCommand::W_SCRATCHPAD, 0x4E},
        {OneWireCommand::CPY_SCRATCHPAD, 0x48},
        {OneWireCommand::REC_EEPROM, 0xB8},
        {OneWireCommand::R_PWR_SUPPLY, 0xB4},
        {OneWireCommand::SEARCH_ROM, 0xF0},
        {OneWireCommand::READ_ROM, 0x33},
        {OneWireCommand::MATCH_ROM, 0x55},
        {OneWireCommand::SKIP_ROM, 0xCC}};

    OneWire::OneWire(io::GpioPin & pin, TIM_HandleTypeDef &tim) : pin_(pin), tim_(tim), logger_("OneWire")
    {
        HAL_TIM_Base_Start(&tim_);// Start the delay timer

        // 1-Wire bit bang initialization
        setBusOutputDirection();
        pin_.set();
        HAL_Delay(100);
        pin_.reset();
        HAL_Delay(100);
        pin_.set();
        HAL_Delay(200);
    }

    void OneWire::delay(uint16_t us) const
    {
        tim_.Instance->CNT = 0;
        while (tim_.Instance->CNT <= us) {}
    }

    void OneWire::setBusInputDirection()
    {
        GPIO_InitTypeDef gpio_init;
        gpio_init.Mode = GPIO_MODE_INPUT;         // Set as input
        gpio_init.Pull = GPIO_NOPULL;             // No pullup - the pullup resistor is external
        gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM; // Medium GPIO frequency
        pin_.init(gpio_init);                  // Reinitialize
    }

    void OneWire::setBusOutputDirection()
    {
        GPIO_InitTypeDef gpio_init;
        gpio_init.Mode = GPIO_MODE_OUTPUT_OD;     // Set as open-drain output
        gpio_init.Pull = GPIO_NOPULL;             // No pullup - the pullup resistor is external
        gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM; // Medium GPIO frequency
        pin_.init(gpio_init);                  // Reinitialize
    }

    /**
     * 1-Wire bus reset signal
     * @return false - ok, true - error
     */
    bool OneWire::resetBus()
    {
        io::PinState state;

        pin_.reset();   // write bus output low
        setBusOutputDirection();
        delay(480); // Wait for reset

        setBusInputDirection(); // Release the bus by switching to input
        delay(70);

        state = pin_.read(); // Check if bus is low
                             // if it's high - no device is presence on the bus
        delay(410);

        bool status = state == io::PinState::SET;
        if(status)
        {
            logger_.error() << "Error occurred when resetting bus.";
        }

        return status;
    }

    /**
     * Write single bit
     */
    void OneWire::writeBit(bool bit)
    {
        if (bit)// Send '1',
        {
            pin_.reset(); // Set the bus low
            setBusOutputDirection();
            delay(6);

            setBusInputDirection();// Release bus - bit high by pullup
            delay(64);
        }
        else// Send '0'
        {
            pin_.reset();// Set the bus low
            setBusOutputDirection();
            delay(60);

            setBusInputDirection();// Release bus - bit high by pullup
            delay(10);
        }
    }

    /**
     * Write byte
     */
    void OneWire::writeByte(uint8_t byte)
    {
        for(uint8_t i = 0; i < 8; ++i, byte >>= 1)
            writeBit(byte & 1); // LSB first
    }

    /**
     * Read single bit
     */
    bool OneWire::readBit()
    {
        io::PinState state;

        pin_.reset(); // Set low to initiate reading
        setBusOutputDirection();
        delay(2);

        setBusInputDirection(); // Release bus for Slave response
        delay(10);

        state = pin_.read(); // Read pin state

        delay(50);// Wait for end of read cycle

        return state == io::PinState::SET;
    }

    /**
     * Read byte
     */
    uint8_t OneWire::readByte()
    {
    	uint8_t i = 8, byte = 0;

		do{
			byte >>= 1;
			byte |= (static_cast<uint8_t>(readBit()) << 7); // LSB first
		} while(--i);

		return byte;
    }

    void OneWire::resetSearch()
    {
        last_discrepancy_ = 0;
        last_device_flag_ = 0;
    }

    uint8_t OneWire::search(OneWireCommand commandId)
    {
        uint8_t id_bit_number;
        uint8_t last_zero, rom_byte_number, search_result;
        uint8_t id_bit, cmp_id_bit;
        uint8_t rom_byte_mask, search_direction;

        id_bit_number = 1;
        last_zero = 0;
        rom_byte_number = 0;
        rom_byte_mask = 1;
        search_result = 0;

        if (!last_device_flag_)// If last device flag is not set
        {
            if (resetBus())// Reset bus
            {
                resetSearch(); // If error while reset - reset search results
                return 0;
            }

            writeByte(commands[commandId]);// Send searching command

            // Searching loop, Maxim APPLICATION NOTE 187
            do
            {
                id_bit = readBit(); // read a bit 1
                cmp_id_bit = readBit(); // read the complement of bit 1

                if (id_bit && cmp_id_bit) // 11 - data error
                {
                    break;
                }
                else if (id_bit != cmp_id_bit) // 01 or 10 - set direction
                {
                    search_direction = id_bit; // Bit write value for search
                }
                else // 00 - 2 devices
                {
                    // Table 3. Search Path Direction
                    if (id_bit_number < last_discrepancy_)
                    {
                        search_direction = ((rom_address_[rom_byte_number] & rom_byte_mask) > 0);
                    }
                    else
                    {
                        // If bit is equal to last - pick 1
                        // If not - then pick 0
                        search_direction = (id_bit_number == last_discrepancy_);
                    }

                    if (search_direction == 0) // If 0 was picked, write it to LastZero
                    {
                        last_zero = id_bit_number;
                    }
                }

                    if (search_direction == 1)
                        rom_address_[rom_byte_number] |= rom_byte_mask; // Set the bit in the ROM byte rom_byte_number
                    else
                        rom_address_[rom_byte_number] &= ~rom_byte_mask; // clearScreen the bit in the ROM byte rom_byte_number

                    writeBit(search_direction); // Search direction write bit

                    id_bit_number++; // Next bit search - increase the id
                    rom_byte_mask <<= 1; // Shift the mask for next bit

                    if (rom_byte_mask == 0)// If the mask is 0, it says the whole byte is read
                    {
                        rom_byte_number++;// Next byte number
                        rom_byte_mask = 1;// Reset the mask - first bit
                    }
            }
            while (rom_byte_number < 8); // read 8 bytes

            if (id_bit_number >= 65 )// If all read bits number is below 65 (8 bytes)
            {
                last_discrepancy_ = last_zero;

                if (last_discrepancy_ == 0)// If last discrepancy is 0 - last device found
                {
                    last_device_flag_ = 1;// Set the flag
                }

                search_result = 1;// Searching successful
            }
        }

        // If no device is found - reset search data and return 0
        if (!search_result || !rom_address_[0])
        {
            last_discrepancy_ = 0;
            last_device_flag_ = 0;
            search_result = 0;
        }

        return search_result;
    }

    /**
     * Find device on 1-Wire bus
     *
     * @param is_first reset search if first
     * @return true - device found, false - no device
     */
    bool OneWire::findDevice(bool is_first)
    {
        if(is_first)
            resetSearch();

        return search(OneWireCommand::SEARCH_ROM);
    }

    /**
     * Select a device on bus by address
     *
     * @param addr OneWire address
     */
    void OneWire::selectDevice(OneWireAddress& addr)
    {
        writeByte(commands[OneWireCommand::MATCH_ROM]);// Match ROM command

        for (auto& ch : addr)
            writeByte( ch);
    }

    //
    //	Get the ROM of found device
    //

    /**
     * Get address of found device
     *
     * @return address of last found device
     */
    OneWireAddress OneWire::getDeviceAddress()
    {
        return rom_address_;
    }

    /**
     * Calculate CRC8 from data
     *
     * @param addr
     * @param len
     * @return
     */
    uint8_t OneWire::calculateCrc8(const uint8_t *addr, uint8_t len) {
        uint8_t crc = 0, inbyte, i, mix;

        while (len--) {
            inbyte = *addr++;
            for (i = 8; i; i--) {
                mix = (crc ^ inbyte) & 0x01;
                crc >>= 1;
                if (mix) {
                    crc ^= 0x8C;
                }
                inbyte >>= 1;
            }
        }

        return crc;
    }

    uint8_t OneWire::calculateCrc8(const std::vector<uint8_t>& data)
    {
        return calculateCrc8(data.data(), data.size());
    }

} //  namespace communication
