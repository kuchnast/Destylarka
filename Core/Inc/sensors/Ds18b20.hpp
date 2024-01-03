#ifndef	SENSORS_DS18B20_HPP
#define	SENSORS_DS18B20_HPP

#include "communication/Onewire.hpp"
#include <config/SensorsDs18b20.hpp>
#include <io/Logger.hpp>

#include <optional>

namespace sensors {

using namespace config;
using namespace communication;

struct Ds18b20
{
    OneWireAddress address;
    float temperature;
    bool is_valid;

    explicit Ds18b20(const OneWireAddress& address);
};

class Ds18b20Collection
{
public:
    Ds18b20Collection() = delete;

    explicit Ds18b20Collection(OneWire& onewire, bool use_crc = false);

    bool addSensor(Ds18b20NameId id, const OneWireAddress& address, uint8_t resolution);

    bool addSensors(const std::vector<Ds18b20Sensor>& sensors, uint8_t resolution);

    bool checkFamilyCode(OneWireAddress &address);

    void startRangingOne(Ds18b20NameId id);

    void startRangingAll();

    bool readOne(Ds18b20NameId id);

    bool readAll();

    size_t size() const;

    bool allDone();

    std::optional<float> getTemperatureMaybe(Ds18b20NameId id);

    uint8_t getResolution(Ds18b20NameId id);

    bool setResolution(Ds18b20NameId id, uint8_t resolution);

private:
    std::optional<float> convertMeasurementToTemperatureMaybe(uint16_t meas, uint8_t resolution);

    static constexpr uint8_t DS18B20_CMD_ALARMSEARCH = 0xEC;
    static constexpr uint8_t DS18B20_CMD_CONVERTTEMP = 0x44;

    const std::map<uint8_t, float> ds_resolution_step{
        {9, 0.0625},
        {10, 0.125},
        {11, 0.25},
        {12, 0.5}};

    std::map<Ds18b20NameId, Ds18b20> sensors_;
    OneWire & onewire_;
    bool use_crc_;
    io::Logger logger_;
};

}
#endif

