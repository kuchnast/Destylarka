#ifndef	SENSORS_DS18B20_HPP
#define	SENSORS_DS18B20_HPP

#include "communication/Onewire.hpp"
#include <config/SensorsDs18b20.hpp>

namespace sensors {

struct Ds18b20
{
    communication::OneWireAddress address;
    float temperature;
    bool is_valid;
};

enum class Ds18b20Resolution {
    R9bits = 9,
    R10bits = 10,
    R11bits = 11,
    R12bits = 12
};

class Ds18b20Collection {
public:

    explicit Ds18b20Collection(communication::OneWire & onewire, bool use_crc = false);

    bool checkFamilyCode(communication::OneWireAddress &address);

    void startRangingOne(config::Ds18b20NameId id);

    void startRangingAll();

    bool readOne(config::Ds18b20NameId id);

    bool readAll();

    [[nodiscard]] size_t size() const;

    void init(Ds18b20Resolution resolution);

    uint8_t getResolution(uint8_t number);

    uint8_t setResolution(uint8_t number, Ds18b20Resolution resolution);



    uint8_t allDone(void);

    void getROM(uint8_t number, uint8_t *ROM);

    void writeROM(uint8_t number, uint8_t *ROM);


    uint8_t getTemperatureByNumber(uint8_t number, float *destination);

    uint8_t getTemperatureById(io::Ds18b20NameId id, float *destination);

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

    std::map<config::Ds18b20NameId, Ds18b20> sensors_;
    communication::OneWire & onewire_;
    bool use_crc_;
};

}
#endif

