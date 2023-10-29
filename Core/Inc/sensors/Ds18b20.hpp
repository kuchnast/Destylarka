#ifndef	SENSORS_DS18B20_HPP
#define	SENSORS_DS18B20_HPP

#include "communication/Onewire.hpp"
#include <config/SensorsDs18b20.hpp>

namespace sensors {

    struct Ds18b20 {
        io::Ds18b20Sensor sensor;
        float temperature;
        bool validDataFlag;
    };

    enum class Ds18b20Resolution {
        R9bits = 9,
        R10bits = 10,
        R11bits = 11,
        R12bits = 12
    };

    class SensorsCollectionDs18b20 {
    public:

        /**
         *
         * @param gpio
         * @param pin
         * @param resolution
         * @param use_crc
         */
        void init(GPIO_TypeDef * gpio, uint16_t pin, Ds18b20Resolution resolution, bool use_crc = false);

        /**
         * Get the sensor resolution
         *
         * @param number
         * @return
         */
        uint8_t getResolution(uint8_t number);

        /**
         * Set the sensor resolution
         *
         * @param number
         * @param resolution
         * @return
         */
        uint8_t setResolution(uint8_t number, Ds18b20Resolution resolution);

        /**
         * Start conversion of one sensor
         *
         * @param number
         * @return
         */
        uint8_t start(uint8_t number);

        /**
         * Start conversion for all sensors
         */
        void startAll();

        /**
         * Read one sensor
         *
         * @param number
         * @param destination
         * @return
         */
        uint8_t read(uint8_t number, float *destination);

        /**
         * Read all connected sensors
         */
        void readAll(void);

        /**
         * Check if ROM address is DS18B20 family
         *
         * @param ROM
         * @return
         */
        uint8_t is(uint8_t *ROM);

        /**
         * Check if all sensor's conversion is done
         *
         * @return
         */
        uint8_t allDone(void);

        /**
         * Get sensor's ROM from 'number' position
         *
         * @param number
         * @param ROM
         */
        void getROM(uint8_t number, uint8_t *ROM);

        /**
         * Write a ROM to 'number' position in sensors table
         *
         * @param number
         * @param ROM
         */
        void writeROM(uint8_t number, uint8_t *ROM);

        /**
         * Returns quantity of connected sensors
         *
         * @return
         */
        uint8_t quantity(void);

        /**
         * Returns not 0 if read data is invalid
         *
         * @param number
         * @param destination
         * @return
         */
        uint8_t getTemperatureByNumber(uint8_t number, float *destination);

        /**
         * Returns not 0 if read data is invalid
         *
         * @param id
         * @param destination
         * @return
         */
        uint8_t getTemperatureById(io::Ds18b20NameId id, float *destination);

        /**
         *
         * @param rom1
         * @param rom2
         * @return
         */
        uint8_t compareROMs(uint8_t *rom1, uint8_t *rom2);

    private:
        static constexpr uint8_t DS18B20_FAMILY_CODE = 0x28;
        static constexpr uint8_t DS18B20_CMD_ALARMSEARCH = 0xEC;
        static constexpr uint8_t DS18B20_CMD_CONVERTTEMP = 0x44;

        static constexpr float DS18B20_STEP_12BIT = 0.0625;
        static constexpr float DS18B20_STEP_11BIT = 0.125;
        static constexpr float DS18B20_STEP_10BIT = 0.25;
        static constexpr float DS18B20_STEP_9BIT = 0.5;

        static constexpr uint8_t DS18B20_RESOLUTION_R1	= 6; // Resolution bit R1
        static constexpr uint8_t DS18B20_RESOLUTION_R0	= 5; // Resolution bit R0

        static constexpr uint8_t DS18B20_DATA_LEN_WITH_CRC	= 9;
        static constexpr uint8_t DS18B20_DATA_LEN_WITHOUT_CRC = 5;

        std::vector<Ds18b20> sensors_;
    };

}
#endif

