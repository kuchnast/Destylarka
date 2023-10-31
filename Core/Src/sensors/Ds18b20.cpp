
#include <sensors/Ds18b20.hpp>

namespace sensors {

    uint8_t OneWireDevices;
    uint8_t TempSensorCount = 0;

    Ds18b20Collection::Ds18b20Collection(communication::OneWire & onewire, bool use_crc)
        : onewire_(onewire), use_crc_(use_crc)
    {}

    /**
     * Check if OneWire device is DS18B20 sensor
     *
     * @param address OneWire device address
     * @return true - DS18B20 sensor, false - something else
     */
    bool Ds18b20Collection::checkFamilyCode(communication::OneWireAddress &address)
    {
        return address[0] == DS18B20_FAMILY_CODE;
    }

    /**
     * Start conversion of one sensor
     *
     * @param id ds18b20 sensor id
     */
    void Ds18b20Collection::startRangingOne(config::Ds18b20NameId id)
    {
        onewire_.resetBus();
        onewire_.selectDevice(sensors_[id].address);
        onewire_.writeByte(DS18B20_CMD_CONVERTTEMP);
    }

    /**
     * Start conversion of all connected sensors
     */
    void Ds18b20Collection::startRangingAll()
    {
        using namespace communication;

        onewire_.resetBus(); // Reset the bus
        onewire_.writeByte(OneWire::commands[OneWireCommand::SKIP_ROM]); // Skip ROM command
        onewire_.writeByte(DS18B20_CMD_CONVERTTEMP); // Start conversion on all sensors
    }

    /**
     * Read temperature when ranging finished
     * @param id ds18b20 sensor id
     * @return true - error occurred, false - temperature read
     */
    bool Ds18b20Collection::readOne(config::Ds18b20NameId id)
    {
        using namespace communication;

        uint16_t temperature;
        uint8_t resolution;
        float result;
        uint8_t i = 0;
        uint8_t data[DS18B20_DATA_LEN];
        uint8_t crc;

        if (!onewire_.readBit()) // Check if the bus is released
            return true; // Busy bus - conversion is not finished

        onewire_.resetBus(); // Reset the bus
        onewire_.selectDevice(sensors_[id].address);
        onewire_.writeByte(OneWire::commands[OneWireCommand::R_SCRATCHPAD]); // Skip ROM command

        for (i = 0; i < DS18B20_DATA_LEN; i++)// read scratchpad
            data[i] = OneWire_ReadByte(&OneWire);

        if(use_crc_)
        {
            crc = OneWire_CRC8(data, 8);// CRC calculation

            if (crc != data[8])
                return 0;// CRC invalid
        }

        temperature = data[0] | (data[1] << 8);// temperature is 16-bit length

        onewire_.resetBus();// Reset the bus

        resolution = ((data[4] & 0x60) >> 5) + 9;// Sensor's resolution from scratchpad's byte 4

        switch (resolution)// Chceck the correct value dur to resolution
        {
            case DS18B20_Resolution_9bits:
                result = temperature * (float) DS18B20_STEP_9BIT;
                break;
            case DS18B20_Resolution_10bits:
                result = temperature * (float) DS18B20_STEP_10BIT;
                break;
            case DS18B20_Resolution_11bits:
                result = temperature * (float) DS18B20_STEP_11BIT;
                break;
            case DS18B20_Resolution_12bits:
                result = temperature * (float) DS18B20_STEP_12BIT;
                break;
            default:
                result = 0xFF;
        }

        *destination = result;

        return 1;//temperature valid
    }

    bool Ds18b20Collection::readAll()
    {

    }

    /**
     * Size of sensors collection
     *
     * @return number of sensors
     */
    size_t Ds18b20Collection::size() const
    {
        return sensors_.size();
    }

    /**
     * Read one sensor
     *
     * @param number
     * @param destination
     * @return
     */
    uint8_t DS18B20_Read(uint8_t number, float *destination) {
        if (number >= TempSensorCount)// If read sensor is not availible
            return 0;

        uint16_t temperature;
        uint8_t resolution;
        float result;
        uint8_t i = 0;
        uint8_t data[DS18B20_DATA_LEN];
#ifdef _DS18B20_USE_CRC
        uint8_t crc;

#endif


        if (!DS18B20_Is((uint8_t *) &ds18b20[number].address))// Check if sensor is DS18B20 family
            return 0;

        if (!OneWire_ReadBit(&OneWire))// Check if the bus is released
            return 0;                  // Busy bus - conversion is not finished

        onewire_.resetBus();                                                  // Reset the bus
       onewire_.selectDevice((uint8_t *) &ds18b20[number].address);// Select the sensor by ROM
        OneWire_WriteByte(&OneWire, ONEWIRE_CMD_RSCRATCHPAD);                     // read scratchpad command

        for (i = 0; i < DS18B20_DATA_LEN; i++)// read scratchpad
            data[i] = OneWire_ReadByte(&OneWire);

#ifdef _DS18B20_USE_CRC
        crc = OneWire_CRC8(data, 8);// CRC calculation

        if (crc != data[8])
            return 0;// CRC invalid
#endif
        temperature = data[0] | (data[1] << 8);// temperature is 16-bit length

        onewire_.resetBus();// Reset the bus

        resolution = ((data[4] & 0x60) >> 5) + 9;// Sensor's resolution from scratchpad's byte 4

        switch (resolution)// Chceck the correct value dur to resolution
        {
            case DS18B20_Resolution_9bits:
                result = temperature * (float) DS18B20_STEP_9BIT;
                break;
            case DS18B20_Resolution_10bits:
                result = temperature * (float) DS18B20_STEP_10BIT;
                break;
            case DS18B20_Resolution_11bits:
                result = temperature * (float) DS18B20_STEP_11BIT;
                break;
            case DS18B20_Resolution_12bits:
                result = temperature * (float) DS18B20_STEP_12BIT;
                break;
            default:
                result = 0xFF;
        }

        *destination = result;

        return 1;//temperature valid
    }

    /**
* Get the sensor resolution
*
* @param number
* @return
*/
    uint8_t DS18B20_GetResolution(uint8_t number) {
        if (number >= TempSensorCount)
            return 0;

        uint8_t conf;

        if (!DS18B20_Is((uint8_t *) &ds18b20[number].address))
            return 0;

        onewire_.resetBus();                                                  // Reset the bus
       onewire_.selectDevice((uint8_t *) &ds18b20[number].address);// Select the sensor by ROM
        OneWire_WriteByte(&OneWire, ONEWIRE_CMD_RSCRATCHPAD);                     // read scratchpad command

        OneWire_ReadByte(&OneWire);
        OneWire_ReadByte(&OneWire);
        OneWire_ReadByte(&OneWire);
        OneWire_ReadByte(&OneWire);

        conf = OneWire_ReadByte(&OneWire);// Register 5 is the configuration register with resolution
        conf &= 0x60;                     // Mask two resolution bits
        conf >>= 5;                       // Shift to left
        conf += 9;                        // Get the result in number of resolution bits

        return conf;
    }

    /**
     * Set the sensor resolution
     *
     * @param number
     * @param resolution
     * @return
     */
    uint8_t DS18B20_SetResolution(uint8_t number, DS18B20_Resolution_t resolution) {
        if (number >= TempSensorCount)
            return 0;

        uint8_t th, tl, conf;
        if (!DS18B20_Is((uint8_t *) &ds18b20[number].address))
            return 0;

        onewire_.resetBus();                                                  // Reset the bus
       onewire_.selectDevice((uint8_t *) &ds18b20[number].address);// Select the sensor by ROM
        OneWire_WriteByte(&OneWire, ONEWIRE_CMD_RSCRATCHPAD);                     // read scratchpad command

        OneWire_ReadByte(&OneWire);
        OneWire_ReadByte(&OneWire);

        th = OneWire_ReadByte(&OneWire);  // Writing to scratchpad begins from the temperature alarms bytes
        tl = OneWire_ReadByte(&OneWire);  // 	so i have to store them.
        conf = OneWire_ReadByte(&OneWire);// Config byte

        if (resolution == DS18B20_Resolution_9bits)// Bits setting
        {
            conf &= ~(1 << DS18B20_RESOLUTION_R1);
            conf &= ~(1 << DS18B20_RESOLUTION_R0);
        } else if (resolution == DS18B20_Resolution_10bits) {
            conf &= ~(1 << DS18B20_RESOLUTION_R1);
            conf |= 1 << DS18B20_RESOLUTION_R0;
        } else if (resolution == DS18B20_Resolution_11bits) {
            conf |= 1 << DS18B20_RESOLUTION_R1;
            conf &= ~(1 << DS18B20_RESOLUTION_R0);
        } else if (resolution == DS18B20_Resolution_12bits) {
            conf |= 1 << DS18B20_RESOLUTION_R1;
            conf |= 1 << DS18B20_RESOLUTION_R0;
        }

        onewire_.resetBus();                                                  // Reset the bus
       onewire_.selectDevice((uint8_t *) &ds18b20[number].address);// Select the sensor by ROM
        OneWire_WriteByte(&OneWire, ONEWIRE_CMD_WSCRATCHPAD);                     // write scratchpad command

        OneWire_WriteByte(&OneWire, th);// write 3 bytes to scratchpad
        OneWire_WriteByte(&OneWire, tl);
        OneWire_WriteByte(&OneWire, conf);

        onewire_.resetBus();                                                  // Reset the bus
       onewire_.selectDevice((uint8_t *) &ds18b20[number].address);// Select the sensor by ROM
        OneWire_WriteByte(&OneWire, ONEWIRE_CMD_CPYSCRATCHPAD);                   // Copy scratchpad to EEPROM

        return 1;
    }



    /**
     * Check if all sensor's conversion is done
     *
     * @return
     */
    uint8_t DS18B20_AllDone(void) {
        return OneWire_ReadBit(&OneWire);// Bus is down - busy
    }

    /**
     * Read all connected sensors
     */
    void DS18B20_ReadAll(void) {
        uint8_t i;

        if (DS18B20_AllDone()) {
            for (i = 0; i < TempSensorCount; i++)// All detected sensors loop
            {
                ds18b20[i].validDataFlag = 0;

                if (DS18B20_Is((uint8_t *) &ds18b20[i].address)) {
                    ds18b20[i].validDataFlag = DS18B20_Read(i, &ds18b20[i].temperature);// read single sensor
                }
            }
        }
    }

    /**
     * Get sensor's ROM from 'number' position
     *
     * @param number
     * @param ROM
     */
    void DS18B20_GetROM(uint8_t number, uint8_t *ROM) {
        if (number >= TempSensorCount)
            number = TempSensorCount;

        uint8_t i;

        for (i = 0; i < 8; i++)
            ROM[i] = ds18b20[number].address[i];
    }

    /**
     * Write a ROM to 'number' position in sensors table
     *
     * @param number
     * @param ROM
     */
    void DS18B20_WriteROM(uint8_t number, uint8_t *ROM) {
        if (number >= TempSensorCount)
            return;

        uint8_t i;

        for (i = 0; i < 8; i++)
            ds18b20[number].address[i] = ROM[i];// write ROM into sensor's structure
    }

    /**
     * Returns quantity of connected sensors
     *
     * @return
     */
    uint8_t DS18B20_Quantity(void) {
        return TempSensorCount;
    }

    /**
     * Returns not 0 if read data is invalid
     *
     * @param number
     * @param destination
     * @return
     */
    uint8_t DS18B20_GetTemperatureByNumber(uint8_t number, float *destination) {
        if (!ds18b20[number].validDataFlag)
            return 1;

        *destination = ds18b20[number].temperature;
        return 0;
    }

    /**
     * Returns not 0 if read data is invalid
     *
     * @param id
     * @param destination
     * @return
     */
    uint8_t DS18B20_GetTemperatureById(DsNamesId id, float *destination) {
        for (int i = 0; i < DS_SENSORS_NUM; ++i) {
            if (ds18b20[i].nameId == id) {
                if (!ds18b20[i].validDataFlag)
                    return 1;

                *destination = ds18b20[i].temperature;
                return 0;
            }
        }

        return 2;
    }

    void DS18B20_Init(DS18B20_Resolution_t resolution) {
        uint8_t next = 0, i = 0, j, k;
        OneWire_Init(&OneWire, DS18B20_GPIO_Port, DS18B20_Pin);// init OneWire bus

        next = OneWire_First(&OneWire);// Search first OneWire device
        while (next) {
            TempSensorCount++;
            OneWire_GetFullROM(&OneWire, (uint8_t *) &ds18b20[i].address);// Get the ROM of next sensor

            for (k = 0; k < DS_SENSORS_NUM; ++k) {
                if (DS18B20_CompareROMs((uint8_t *) &known_ds_sensors[k].address, (uint8_t *) &ds18b20[i].address))
                    ds18b20[i].nameId = known_ds_sensors[k].name_id;
            }

            ++i;

            next = OneWire_Next(&OneWire);
            if (TempSensorCount >= _DS18B20_MAX_SENSORS)// More sensors than set maximum is not allowed
                break;
        }

        for (j = 0; j < i; j++) {
            DS18B20_SetResolution(j, resolution);// Set the initial resolution to sensor

            DS18B20_StartAll();// Start conversion on all sensors
        }
    }

    uint8_t DS18B20_CompareROMs(uint8_t *rom1, uint8_t *rom2) {
        for (uint8_t i = 0; i < 8; ++i) {
            if (rom1[i] != rom2[i])
                return 0;
        }

        return 1;
    }

}
