
#include <sensors/Ds18b20.hpp>

namespace sensors {

    Ds18b20::Ds18b20(const OneWireAddress &address) : address(address), temperature(0.0), is_valid(false)
    {
    }

    Ds18b20Collection::Ds18b20Collection(OneWire &onewire, bool use_crc)
        : onewire_(onewire), use_crc_(use_crc), logger_("Ds18b20Collection") {}

    /**
     * Find and add ds18b20 to known sensors
     *
     * @param id ds18b20 sensor id
     * @param address OneWire sensor address
     * @param resolution as number of bits with measurement read from sensor
     * @return true - sensor not found or error, false - sensor added
     */
    bool Ds18b20Collection::addSensor(Ds18b20NameId id, const OneWireAddress &address, uint8_t resolution) {
        OneWireAddress device_address;

        if (onewire_.findDevice(true)) {
            do {
                device_address = onewire_.getDeviceAddress();
                if (device_address == address)
                {
                    if(sensors_.try_emplace(id, address).second)
                    {
                        bool status = setResolution(id, resolution);
                        if(status)
                            logger_.error() << "Sensor added but resolution " << std::to_string(resolution) << " cant be set for sensor " << config::toString(id);
                        return status;
                    }
                    break;
                }
            } while (onewire_.findDevice());
        }

        return true;
    }

    /**
     * Find and add multiple ds18b20 to known sensors
     *
     * @param id ds18b20 sensor id
     * @param address OneWire sensor address
     * @param resolution as number of bits with measurement read from sensor
     * @return true - one or many sensors not found or error, false - all sensors added
     */
    bool Ds18b20Collection::addSensors(const std::vector<Ds18b20Sensor>& sensors, uint8_t resolution)
    {
        for(const auto& sensor : sensors)
            addSensor(sensor.name_id, sensor.address, resolution);

        return size() != sensors.size();
    }

    /**
     * Check if OneWire device is DS18B20 sensor
     *
     * @param address OneWire device address
     * @return true - DS18B20 sensor, false - something else
     */
    bool Ds18b20Collection::checkFamilyCode(OneWireAddress &address) {
        static constexpr uint8_t DS18B20_FAMILY_CODE = 0x28;

        return address[0] == DS18B20_FAMILY_CODE;
    }

    /**
     * Start conversion of one sensor
     *
     * @param id ds18b20 sensor id
     */
    void Ds18b20Collection::startRangingOne(Ds18b20NameId id) {
        auto sensor = sensors_.find(id);

        if(sensor != sensors_.end())
        {
            onewire_.resetBus();
            onewire_.selectDevice(sensor->second.address);
            onewire_.writeByte(DS18B20_CMD_CONVERTTEMP);
        }
    }

    /**
     * Start conversion of all connected sensors
     */
    void Ds18b20Collection::startRangingAll() {
        onewire_.resetBus();                                            // Reset the bus
        onewire_.writeByte(OneWire::commands[OneWireCommand::SKIP_ROM]);// Skip ROM command
        onewire_.writeByte(DS18B20_CMD_CONVERTTEMP);                    // Start conversion on all sensors
    }

    /**
     * Read temperature when ranging finished
     * @param id ds18b20 sensor id
     * @return true - error occurred, false - temperature read
     */
    bool Ds18b20Collection::readOne(Ds18b20NameId id) {
        static constexpr uint8_t DATA_LEN_WITH_CRC = 9;
        static constexpr uint8_t DATA_LEN_WITHOUT_CRC = 5;

        uint16_t measurement;
        uint8_t resolution;
        std::vector<uint8_t> data;
        auto sensor = sensors_.find(id);

        if(sensor == sensors_.end())
        {
            logger_.error() << "Can't read temperature - sensor id " << config::toString(id) << " not found.";
            return true;
        }

        if (use_crc_)
            data.resize(DATA_LEN_WITH_CRC);
        else
            data.resize(DATA_LEN_WITHOUT_CRC);

        if (!onewire_.readBit())// Check if the bus is released
        {
            sensor->second.is_valid = false;
            logger_.error() << "Can't read temperature for sensor id " << config::toString(id) << " - bus is busy.";
            return true;
        }

        onewire_.resetBus();// Reset the bus
        onewire_.selectDevice(sensor->second.address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::R_SCRATCHPAD]);// Skip ROM command

        for (auto &el: data)// read scratchpad
            el = onewire_.readByte();

        if (use_crc_) {
            uint8_t dev_crc = data.back();
            data.pop_back();
            uint8_t crc = OneWire::calculateCrc8(data);// CRC calculation
            logger_.debug() << "Calculated CRC: " << io::num_to_hex(crc) << " From device: " << io::num_to_hex(dev_crc);

            if (crc != dev_crc) {
                sensor->second.is_valid = false;
                logger_.error() << "Can't read temperature for sensor id " << config::toString(id) << " - invalid CRC.";

                auto lg = logger_.debug();
                lg << "Data: ";
                for(auto &val: data)
                    lg << io::num_to_hex(val) << ' ';

                return true;
            }
        }

        onewire_.resetBus();// Reset the bus

        measurement = data[0] | (data[1] << 8);  // temperature is 16-bit length
        resolution = ((data[4] & 0x60) >> 5) + 9;// Sensor's resolution from scratchpad's byte 4
        auto temperature = convertMeasurementToTemperatureMaybe(measurement, resolution);

        if (temperature.has_value()) {
            sensor->second.temperature = temperature.value();
            sensor->second.is_valid = true;
        } else {
            sensor->second.is_valid = false;
            logger_.error() << "Can't convert measurement to temperature for sensor id " << config::toString(id);
            return true;
        }

        return false;// temperature valid
    }

    /**
     * Read temperature from all sensors
     * @return true - error occurred in one or many reads, false - temperature read
     */
    bool Ds18b20Collection::readAll() {
        bool flag = false;

        for (const auto &sensor: sensors_)
            flag |= readOne(sensor.first);

        return flag;
    }

    /**
     * Size of sensors collection
     *
     * @return number of sensors
     */
    size_t Ds18b20Collection::size() const {
        return sensors_.size();
    }

    /**
     * Check if all sensor's conversion is done
     *
     * @return true if all sensors are ready, false if bus is busy
     */
    bool Ds18b20Collection::allDone() {
        return onewire_.readBit();// Bus is down - busy
    }

    /**
     * Get last read temperature from sensor
     *
     * @param id ds18b20 sensor id
     * @return temperature or nullopt if temperature not valid
     */
    std::optional<float> Ds18b20Collection::getTemperatureMaybe(Ds18b20NameId id) {
        auto sensor = sensors_.find(id);

        if(sensor == sensors_.end())
            return std::nullopt;

        return sensor->second.is_valid ? std::optional<float>(sensor->second.temperature) : std::nullopt;
    }

    /**
    * Get the sensor resolution
    *
    * @param id ds18b20 sensor id
    * @return resolution as number of bits with measurement read from sensor
    */
    uint8_t Ds18b20Collection::getResolution(Ds18b20NameId id) {
        uint8_t resolution_raw;
        auto sensor = sensors_.find(id);

        if(sensor == sensors_.end())
            return true;

        onewire_.resetBus();// Reset the bus
        onewire_.selectDevice(sensor->second.address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::R_SCRATCHPAD]);// Skip ROM command

        for (uint8_t i = 0; i < 4; ++i)// skip first 4 bytes
            onewire_.readByte();
        resolution_raw = onewire_.readByte();

        return ((resolution_raw & 0x60) >> 5) + 9;// Sensor's resolution from raw scratchpad's byte
    }

    /**
     * Set the sensor resolution
     *
     * @param id ds18b20 sensor id
     * @param resolution as number of bits with measurement read from sensor
     * @return true if resolution not valid, false if resolution set
     */
    bool Ds18b20Collection::setResolution(Ds18b20NameId id, uint8_t resolution) {
        static constexpr uint8_t DS18B20_RESOLUTION_R1 = 6;// Resolution bit R1
        static constexpr uint8_t DS18B20_RESOLUTION_R0 = 5;// Resolution bit R0

        std::vector<uint8_t> data(5);
        auto sensor = sensors_.find(id);

        if(sensor == sensors_.end())
        {
            logger_.error() << "Can't set resolution - sensor id " << config::toString(id) << " not found.";
            return true;
        }

        if (ds_resolution_step.find(resolution) == ds_resolution_step.end())
        {
            logger_.error() << "Can't set resolution - resolution " << std::to_string(resolution) << " is not valid.";
            return true;
        }

        onewire_.resetBus();// Reset the bus
        onewire_.selectDevice(sensor->second.address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::R_SCRATCHPAD]);// Skip ROM command

        for (auto &byte: data)
            byte = onewire_.readByte();

        if (resolution == 9)// Bits setting
        {
            data[4] &= ~(1 << DS18B20_RESOLUTION_R1);
            data[4] &= ~(1 << DS18B20_RESOLUTION_R0);
        } else if (resolution == 10) {
            data[4] &= ~(1 << DS18B20_RESOLUTION_R1);
            data[4] |= 1 << DS18B20_RESOLUTION_R0;
        } else if (resolution == 11) {
            data[4] |= 1 << DS18B20_RESOLUTION_R1;
            data[4] &= ~(1 << DS18B20_RESOLUTION_R0);
        } else if (resolution == 12) {
            data[4] |= 1 << DS18B20_RESOLUTION_R1;
            data[4] |= 1 << DS18B20_RESOLUTION_R0;
        }

        onewire_.resetBus();// Reset the bus
        onewire_.selectDevice(sensor->second.address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::W_SCRATCHPAD]);// write scratchpad command

        for (auto it = data.begin() + 2; it != data.end(); ++it)// write 3 bytes to scratchpad
            onewire_.writeByte(*it);

        onewire_.resetBus();// Reset the bus
        onewire_.selectDevice(sensor->second.address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::CPY_SCRATCHPAD]);// copy scratchpad to EEPROM

        return false;
    }

    /**
     * Convert raw measurement and resolution to temperature
     *
     * @param meas measurement from sensor
     * @param resolution bit of resolution set on device
     * @return temperature or nullopt if resolution not found
     */
    std::optional<float> Ds18b20Collection::convertMeasurementToTemperatureMaybe(uint16_t meas, uint8_t resolution) {
        auto it = ds_resolution_step.find(resolution);

        if (it != ds_resolution_step.end())
            return static_cast<float>(meas) * it->second;
        else
            return std::nullopt;
    }
}
