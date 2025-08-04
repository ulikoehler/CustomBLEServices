#pragma once
#include "CustomBLE/Characteristic.hpp"
#include <vector>
#include <memory>
#include <cstddef>

namespace CustomBLE {

class CharacteristicsManager {
public:
    struct CharacteristicEntry {
        std::shared_ptr<Characteristic> characteristic;
        ble_gatt_chr_def chr_def;
    };

private:
    std::vector<CharacteristicEntry> entries;
    std::vector<ble_gatt_chr_def> chr_defs;
public:
    // Add public accessor for entries
    const std::vector<CharacteristicEntry>& get_entries() const { return entries; }

public:
    /**
     * @brief Add a new characteristic to the manager.
     * @param characteristic Unique pointer to a Characteristic
     */
    void add_characteristic(std::shared_ptr<Characteristic> characteristic);

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

    /**
     * @brief Get pointer to the array of ble_gatt_chr_def for service definition.
     * The last element is always the end marker.
     */
    ble_gatt_chr_def* get_chr_defs();

    /**
     * @brief Get the number of managed characteristics (excluding end marker).
     */
    size_t size() const;

    /**
     * @brief Generate a string overview of all characteristics.
     */
    std::string overview() const;

    /**
     * @brief Print the overview using printf().
     */
    void print() const;

private:
    /**
     * @brief Update the internal chr_defs vector, ensuring the last element is the end marker.
     */
    void update_chr_defs();
};

} // namespace CustomBLE
