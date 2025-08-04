
#pragma once
#include "CustomBLE/Characteristic.hpp"
#include <vector>
#include <memory>
#include <cstddef>
#include <string>
namespace CustomBLE {

class ServiceCharacteristicsManager {
public:
    std::string overview() const;
    void print() const;
    struct CharacteristicEntry {
        std::unique_ptr<Characteristic> characteristic;
        ble_gatt_chr_def chr_def;
    };

private:
    std::vector<CharacteristicEntry> entries;
    std::vector<ble_gatt_chr_def> chr_defs;

public:
    void add_characteristic(std::unique_ptr<Characteristic> characteristic);

    /**
     * @brief Emplace a new characteristic inline (constructs and adds).
     * @param characteristic_uuid UUID of the characteristic
     * @param initial_value Initial value
     * @param read_cb Optional read callback
     * @param write_cb Optional write callback
     * @return Reference to the newly added Characteristic
     */
    Characteristic& emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                           const std::string& initial_value,
                                           Characteristic::ReadCallback read_cb = nullptr,
                                           Characteristic::WriteCallback write_cb = nullptr);
    ble_gatt_chr_def* get_chr_defs();
    size_t size() const;

private:
    void update_chr_defs();
};

} // namespace CustomBLE
